#!/usr/bin/env bash
#
# Regenerate every distro's packaging metadata from the two sources of truth:
#   - CMakeLists.txt : version, url, maintainer name, maintainer email
#   - CHANGELOG.md   : the change history (Keep a Changelog format)
#
# Nothing version- or maintainer-specific is hardcoded in the packaging files;
# run this after bumping the version in CMakeLists.txt and adding a CHANGELOG
# entry. It rewrites:
#   - debian/changelog
#   - packaging/rpm/ledspicerui.spec   (Version, Release, %changelog)
#   - packaging/arch/PKGBUILD          (pkgver, pkgrel, sha256sums)
#   - packaging/arch/.SRCINFO
#
# The Arch checksum is fetched from the released tag tarball; before the tag
# exists it falls back to SKIP (the release workflow fills it in later).

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CMAKE="$ROOT/CMakeLists.txt"
CHANGELOG="$ROOT/CHANGELOG.md"
SPEC="$ROOT/packaging/rpm/ledspicerui.spec"
PKGBUILD="$ROOT/packaging/arch/PKGBUILD"
SRCINFO="$ROOT/packaging/arch/.SRCINFO"

RELEASE=1

# ── read metadata from CMakeLists.txt (single source of truth) ───────────────
VERSION=$(sed -n 's/.*project(LEDSpicerUI VERSION \([0-9.]*\).*/\1/p'            "$CMAKE")
URL=$(sed -n 's/.*set(PACKAGE_URL[[:space:]]*"\([^"]*\)".*/\1/p'                 "$CMAKE")
EMAIL=$(sed -n 's/.*set(MAINTAINER_EMAIL[[:space:]]*"\([^"]*\)".*/\1/p'          "$CMAKE")
NAME=$(sed -n 's/.*set(COPYRIGHT[[:space:]]*"[^"]*[0-9]\{4\} \(.*\)").*/\1/p'    "$CMAKE")

[ -n "$VERSION" ] || { echo "ERROR: could not read VERSION from $CMAKE" >&2; exit 1; }
[ -n "$URL" ]     || { echo "ERROR: could not read PACKAGE_URL from $CMAKE" >&2; exit 1; }
[ -n "$EMAIL" ]   || { echo "ERROR: could not read MAINTAINER_EMAIL from $CMAKE" >&2; exit 1; }
[ -n "$NAME" ]    || { echo "ERROR: could not read maintainer name from COPYRIGHT" >&2; exit 1; }

echo ":: version=$VERSION  maintainer=$NAME <$EMAIL>"

# ── changelog helpers ────────────────────────────────────────────────────────
# Print the raw body of one version's block from CHANGELOG.md.
extract_block() { # $1 = version
	awk -v ver="$1" '
		/^## \[[0-9]/ { if (found) exit; if ($0 ~ "\\[" ver "\\]") found=1; next }
		found { print }
	' "$CHANGELOG"
}

# Debian layout:  "### X" -> "  * X",  "- y" -> "    - y"
changes_deb() { # $1 = version
	extract_block "$1" | awk '
		/^### / { sub(/^### /, "  * ");  print; next }
		/^- /   { sub(/^- /,   "    - "); print; next }
	'
}

# RPM layout:  "### X" -> "- X",  "- y" -> "  - y"
changes_rpm() { # $1 = version
	extract_block "$1" | awk '
		/^### / { sub(/^### /, "- ");   print; next }
		/^- /   { sub(/^- /,   "  - "); print; next }
	'
}

version_of() { sed -n 's/^## \[\([0-9.]*\)\].*/\1/p'                     <<<"$1"; }
date_of()    { sed -n 's/.*\][[:space:]]*-[[:space:]]*\([0-9-]*\).*/\1/p' <<<"$1"; }

# ── debian/changelog ─────────────────────────────────────────────────────────
gen_debian() {
	local out="$ROOT/debian/changelog" first=1 header ver d rfc
	mkdir -p "$(dirname "$out")"
	: > "$out"
	while IFS= read -r header; do
		ver=$(version_of "$header"); d=$(date_of "$header")
		if [ -n "$d" ]; then rfc=$(date -R -d "$d" 2>/dev/null || date -R); else rfc=$(date -R); fi
		[ "$first" -eq 1 ] || printf '\n' >> "$out"; first=0
		{
			printf 'ledspicerui (%s-%s) stable; urgency=medium\n\n' "$ver" "$RELEASE"
			changes_deb "$ver"
			printf '\n -- %s <%s>  %s\n' "$NAME" "$EMAIL" "$rfc"
		} >> "$out"
	done < <(grep -E '^## \[[0-9]' "$CHANGELOG")
	echo ":: wrote debian/changelog"
}

# ── packaging/rpm/ledspicerui.spec ───────────────────────────────────────────
gen_rpm() {
	[ -f "$SPEC" ] || { echo ":: skip rpm (no $SPEC yet)"; return; }
	sed -i "s/^Version:.*/Version:        $VERSION/"        "$SPEC"
	sed -i "s/^Release:.*/Release:        $RELEASE%{?dist}/" "$SPEC"
	# %changelog is the final section: drop it and rebuild from CHANGELOG.md.
	sed -i '/^%changelog/,$d' "$SPEC"
	{
		printf '%%changelog\n'
		local first=1 header ver d dstr
		while IFS= read -r header; do
			ver=$(version_of "$header"); d=$(date_of "$header")
			if [ -n "$d" ]; then dstr=$(date -d "$d" "+%a %b %d %Y" 2>/dev/null || date "+%a %b %d %Y"); else dstr=$(date "+%a %b %d %Y"); fi
			[ "$first" -eq 1 ] || printf '\n'; first=0
			printf '* %s %s <%s> - %s-%s\n' "$dstr" "$NAME" "$EMAIL" "$ver" "$RELEASE"
			changes_rpm "$ver"
		done < <(grep -E '^## \[[0-9]' "$CHANGELOG")
	} >> "$SPEC"
	echo ":: wrote $SPEC"
}

# ── packaging/arch/PKGBUILD + .SRCINFO ───────────────────────────────────────
gen_arch() {
	[ -f "$PKGBUILD" ] || { echo ":: skip arch (no $PKGBUILD yet)"; return; }
	sed -i "s/^pkgver=.*/pkgver=$VERSION/" "$PKGBUILD"
	sed -i "s/^pkgrel=.*/pkgrel=$RELEASE/" "$PKGBUILD"

	# Hash the released tag tarball; fall back to SKIP before the tag exists.
	local sum empty
	empty=$(printf '' | sha256sum | cut -d' ' -f1)
	sum=$(curl -fsSL "$URL/archive/refs/tags/$VERSION.tar.gz" 2>/dev/null | sha256sum | cut -d' ' -f1 || true)
	if [ -z "$sum" ] || [ "$sum" = "$empty" ]; then sum=SKIP; fi
	sed -i "s/^sha256sums=.*/sha256sums=('$sum')/" "$PKGBUILD"
	echo ":: wrote $PKGBUILD (sha256sums=$sum)"

	# .SRCINFO field lines must be tab-indented; build it with explicit tabs.
	{
		printf 'pkgbase = ledspicerui\n'
		printf '\tpkgdesc = Graphical configuration editor for LEDSpicer\n'
		printf '\tpkgver = %s\n' "$VERSION"
		printf '\tpkgrel = %s\n' "$RELEASE"
		printf '\turl = %s\n' "$URL"
		printf '\tarch = x86_64\n'
		printf '\tlicense = GPL3\n'
		printf '\tmakedepends = cmake\n'
		printf '\tmakedepends = glib2\n'
		printf '\tmakedepends = libxml2\n'
		printf '\tdepends = gtkmm3\n'
		printf '\tdepends = tinyxml2\n'
		printf '\toptdepends = ledspicer: the LED daemon this UI configures\n'
		printf '\tsource = ledspicerui-%s.tar.gz::%s/archive/refs/tags/%s.tar.gz\n' "$VERSION" "$URL" "$VERSION"
		printf '\tsha256sums = %s\n' "$sum"
		printf '\n'
		printf 'pkgname = ledspicerui\n'
	} > "$SRCINFO"
	echo ":: wrote $SRCINFO"
}

gen_debian
gen_rpm
gen_arch
echo ":: done"
