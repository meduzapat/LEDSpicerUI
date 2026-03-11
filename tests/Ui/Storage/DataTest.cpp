#include <gtest/gtest.h>
#include <gtkmm.h>
#include "Storage/Data.hpp"

using namespace LEDSpicerUI::Ui::Storage;

// Concrete test class to instantiate Data
class TestData : public Data {

public:

	TestData(std::unordered_map<std::string, std::string>& data) : Data(data) {
		ignored.insert("ignored");
	}
	const string getCssClass() const override { return "test-class"; }
};

class DataTest : public ::testing::Test {

protected:

	void SetUp() override {
		std::unordered_map<std::string, std::string> testData = {
			{"name",    "TestItem"},
			{"type",    "button"},
			{"value",   "42"},
			{"ignored", "yes"}
		};
		data = std::make_unique<TestData>(testData);
	}

	void TearDown() override {
		data.reset();
		app.reset();
	}

	Glib::RefPtr<Gtk::Application> app;
	std::unique_ptr<TestData> data;
};

TEST_F(DataTest, GetValue) {
	EXPECT_EQ(data->getValue("name"), "TestItem");
	EXPECT_EQ(data->getValue("nonexistent"), "");
	EXPECT_EQ(data->getValue("nonexistent", "default"), "default");
	EXPECT_EQ(data->getValue("ignored"), "yes");
}

TEST_F(DataTest, SetValue) {
	data->setValue("key", "value");
	EXPECT_EQ(data->getValue("key"), "value");
	data->setValue("key2", "value2");
	data->setValue("key2", "NewName");
	EXPECT_EQ(data->getValue("key2"), "NewName");
}

TEST_F(DataTest, UnSet) {
	data->unSet("key2");
	EXPECT_EQ(data->getValue("key2"), "");
	EXPECT_EQ(data->getValues()->count("key2"), 0);
}

TEST_F(DataTest, Wipe) {
	data->wipe();
	EXPECT_TRUE(data->getValues()->empty());
}

TEST_F(DataTest, ToXMLSimple) {
	string expected = "value=\"42\"\ntype=\"button\"\nname=\"TestItem\"\n";
	EXPECT_EQ(data->toXML(), expected);
	data->unSet("value");
	expected = "type=\"button\"\nname=\"TestItem\"\n";
	EXPECT_EQ(data->toXML(), expected);
}

TEST_F(DataTest, GetValues) {
	const auto values = data->getValues();
	EXPECT_EQ(values->size(), 4);
}

TEST_F(DataTest, createPrettyName) {
	EXPECT_EQ(data->createPrettyName(), "TestItem");
}

TEST_F(DataTest, createUniqueId) {
	EXPECT_EQ(data->createUniqueId(), "TestItem");
}

TEST_F(DataTest, copyValues) {
	StringUMap expected {
		{"name",    "TestItem copy1" },
		{"type",    "button"},
		{"value",   "42"},
		{"ignored", "yes"}
	};
	EXPECT_EQ(data->copyValues(1), expected);
}

TEST_F(DataTest, PropertyOperations) {
	data->setProperty("key1", "value1");
	data->setProperty("key2", "value2");

	EXPECT_EQ("value1", data->getProperty("key1"));
	EXPECT_EQ("value2", data->getProperty("key2"));
	EXPECT_EQ("default", data->getProperty("missing", "default"));
	EXPECT_TRUE(data->hasProperty("key1"));
	EXPECT_FALSE(data->hasProperty("missing"));
}

TEST_F(DataTest, RemoveProperty) {
	data->setProperty("test", "value");
	EXPECT_TRUE(data->hasProperty("test"));

	data->removeProperty("test");
	EXPECT_FALSE(data->hasProperty("test"));
	EXPECT_EQ("", data->getProperty("test"));
}

TEST_F(DataTest, GetAllProperties) {
	data->setProperty("prop1", "val1");
	data->setProperty("prop2", "val2");

	const StringUMap& props = data->getProperties();
	EXPECT_EQ(2, props.size());
	EXPECT_EQ("val1", props.at("prop1"));
	EXPECT_EQ("val2", props.at("prop2"));
}

TEST_F(DataTest, OverwriteProperty) {
	data->setProperty("key", "original");
	EXPECT_EQ("original", data->getProperty("key"));

	data->setProperty("key", "updated");
	EXPECT_EQ("updated", data->getProperty("key"));
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
