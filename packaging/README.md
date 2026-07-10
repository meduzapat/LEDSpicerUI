# Packaging and release workflow

Maintainer runbook for releasing and packaging **LEDSpicer UI**. Publishing to the
PPA, AUR, and COPR relies on `meduzapat`'s own accounts and signing keys, so in
practice only the maintainer runs these steps — this is a memo to self.

`CMakeLists.txt` is the **single source of truth** — version, URL, maintainer
name (from `COPYRIGHT`), and email (`MAINTAINER_EMAIL`) are read from it by
[`generate-changelogs.sh`](generate-changelogs.sh). `CHANGELOG.md` (Keep a
Changelog format) is the changelog source. Never hardcode any of these in the
packaging files — regenerate instead.

## 1. Repository layout

```
LEDSpicerUI/
├── CMakeLists.txt                ← source of truth (version, url, maintainer, email)
├── CHANGELOG.md                  ← changes in Keep a Changelog format
├── debian/                       ← Ubuntu / Debian source package (Launchpad reads this)
│   ├── changelog                 ← generated
│   ├── control
│   ├── rules
│   ├── copyright
│   └── source/format
└── packaging/
    ├── README.md                 ← this file
    ├── generate-changelogs.sh    ← regenerates every distro file from the two sources of truth
    ├── arch/
    │   ├── PKGBUILD              ← pkgver updated by the generator; checksum stays SKIP in-repo
    │   └── .SRCINFO             ← generated (remember to push it to AUR)
    └── rpm/
        └── ledspicerui.spec      ← Version + %changelog updated by the generator
```

Keeping the packaging metadata inside the code repo is required: Launchpad reads
`debian/`, COPR reads `packaging/rpm/ledspicerui.spec`, and the AUR `PKGBUILD` is
sourced from `packaging/arch/`.

## 2. Cut a release

Work from the project root on your release branch.

1. **Bump the version** in `CMakeLists.txt`:
   `project(LEDSpicerUI VERSION x.y.z …)`.
2. **Add a `CHANGELOG.md` entry** in Keep a Changelog format, newest first:
   `## [x.y.z] - YYYY-MM-DD`, using only the subsections with content (Added,
   Changed, Fixed, Removed, Breaking Changes). One concise, user-facing line per
   bullet; cite issue/PR numbers in parentheses.
3. **Run the generator.** Before the tag exists the Arch checksum falls back to
   `SKIP` — that is expected and stays that way in the repo:
   ```bash
   ./packaging/generate-changelogs.sh
   ```
   It rewrites `debian/changelog`, `packaging/rpm/ledspicerui.spec`
   (`Version` + `%changelog`), and `packaging/arch/PKGBUILD` + `.SRCINFO`.
4. **Review, commit, and push**, then open a PR to the default branch and merge.
   The spec and debian changelog must be committed *before* the tag so they travel
   into the release tarball.
5. **Cut the GitHub Release** on the merged commit. This creates the **bare**
   `x.y.z` tag (no `v`) and publishes the source tarball that the AUR checksum and
   the Fedora `Source0` depend on.
6. **Attach `install.sh` as a Release asset.** The one-line installer is *not*
   tracked in the repo — it is uploaded to the Release so the stable URL
   `https://github.com/meduzapat/LEDSpicerUI/releases/latest/download/install.sh`
   always resolves to the current script.

> Tags are bare (`0.1.0`, not `v0.1.0`). If you spot a mistake after cutting the
> Release, delete the Release **and** the tag (`git push --delete origin x.y.z`),
> fix it on the branch, and re-cut — never patch on top of a published tag.

## 3. Publish

With [`.github/workflows/release.yml`](../.github/workflows/release.yml) configured,
**cutting the Release is the only manual trigger** for AUR and COPR. The steps
below are the manual fallback.

### PPA — Ubuntu / Debian
Launchpad builds `debian/` from source. First time, add the remote and create a
recipe (see setup below), then:
```bash
git push launchpad master
```
Recipes don't build on push — enable "Build daily" or click **Request build(s)**.

### AUR — Arch Linux
Now that the tag tarball exists, fill in the real checksum, push to the **AUR repo
only**, then discard the local change so the code repo keeps `SKIP`:
```bash
./packaging/generate-changelogs.sh                    # now fetches the real sha256
cp packaging/arch/PKGBUILD packaging/arch/.SRCINFO "${AUR_DIR}/"
git -C "${AUR_DIR}" commit -am "Update to $(sed -n 's/.*project(LEDSpicerUI VERSION \([0-9.]*\).*/\1/p' CMakeLists.txt)"
git -C "${AUR_DIR}" push
git restore packaging/arch/PKGBUILD packaging/arch/.SRCINFO
```
The checksum never lives in the code repo: the tarball it hashes *contains*
`PKGBUILD`, so writing the hash back into it would invalidate the hash. If it
still prints `SKIP`, GitHub hasn't finished publishing the tag tarball — wait and
re-run.

### COPR — Fedora
Once COPR is set to build from the Git source (one-time, below), cutting the
Release is all that's needed: the webhook makes COPR clone the tag, find
`packaging/rpm/ledspicerui.spec`, and build.

## 4. One-time setup

### Build dependencies
```bash
# Debian / Ubuntu
sudo apt-get install build-essential cmake pkg-config \
    libgtkmm-3.0-dev libtinyxml2-dev libglib2.0-bin libxml2-utils
# Fedora
sudo dnf install gcc-c++ cmake pkgconf gtkmm30-devel tinyxml2-devel glib2-devel libxml2
# Arch
sudo pacman -S --needed base-devel cmake gtkmm3 tinyxml2 glib2 libxml2
```

### PPA / Launchpad
Add the Launchpad git remote and create a source-package recipe pointing at the
release branch. Building requires your GPG signing key registered with Launchpad.
```bash
git remote add launchpad git+ssh://<lp-user>@git.launchpad.net/ledspicerui
```

### AUR
Clone the package repo once; `AUR_DIR` is used in the publish step above.
```bash
git clone ssh://aur@aur.archlinux.org/ledspicerui.git ~/aur/ledspicerui
```
For automation, generate a **dedicated CI key**, register its public half on AUR,
and store the private half in the `AUR_SSH_PRIVATE_KEY` GitHub secret.

### COPR
In the COPR web UI, open the `ledspicerui` project → Packages → New package → SCM:
- Clone URL: `https://github.com/meduzapat/LEDSpicerUI.git`
- Spec file: `packaging/rpm/ledspicerui.spec`
- SRPM build method: **`rpkg`**

Grab the **custom** webhook URL (Settings → Integrations → the `/webhooks/custom/…`
one) and store it in the `COPR_WEBHOOK_URL` GitHub secret so `release.yml` can POST
to it after the Release is cut.

## 5. Test the packages (optional)

```bash
# Debian — build the .deb locally from the repo root
debuild -us -uc

# Arch — validate in a clean chroot
cd packaging/arch && makepkg -si

# Fedora — build and lint (on a Fedora machine)
rpmbuild -ba packaging/rpm/ledspicerui.spec
rpmlint     packaging/rpm/ledspicerui.spec
```
