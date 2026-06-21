#include "test_file_utils.h"


bool TestFileUtils::_createFileWithSize(const QString& path, qint64 size)
{
	QFile file(path);

	if (file.open(QIODevice::WriteOnly))
	{
		file.write(QByteArray(static_cast<int>(size), '\0'));
		file.close();

		return true;
	}

	return false;
}

void TestFileUtils::testEmptyDirectory()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(0));
}

void TestFileUtils::testSingleFile()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	QVERIFY(_createFileWithSize(tempDir.path() + "/test.txt", 100));

	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(100));
}

void TestFileUtils::testMultipleFiles()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	QVERIFY(_createFileWithSize(tempDir.path() + "/file1.txt", 100));
	QVERIFY(_createFileWithSize(tempDir.path() + "/file2.txt", 200));
	QVERIFY(_createFileWithSize(tempDir.path() + "/file3.txt", 300));

	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(600));
}

void TestFileUtils::testNestedDirectories()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	// Struct is:
	// root/
	//		file1.txt (100)
	//		dir1/
	//			file2.txt (200)
	//			dir2/
	//				file3.txt (300)

	QVERIFY(_createFileWithSize(tempDir.path() + "/file1.txt", 100));
	QVERIFY(QDir(tempDir.path()).mkpath("dir1/dir2"));
	QVERIFY(_createFileWithSize(tempDir.path() + "/dir1/file2.txt", 200));
	QVERIFY(_createFileWithSize(tempDir.path() + "/dir1/dir2/file3.txt", 300));

	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(600));
}

void TestFileUtils::testDeepNesting()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	// 100 lvl deep
	QString currentPath = tempDir.path();
	for (int i = 0; i < 100; i++)
	{
		currentPath += "/level" + QString::number(i);
		QVERIFY(QDir(tempDir.path()).mkpath(currentPath));
	}

	// in deepest lvl, create the file.
	QVERIFY(_createFileWithSize(currentPath + "/deep_file.txt", 42));

	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(42));
}

void TestFileUtils::testHiddenFiles()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	// hidden files must be processed.
	QVERIFY(_createFileWithSize(tempDir.path() + "/visible.txt", 100));
	QVERIFY(_createFileWithSize(tempDir.path() + "/.hidden", 200));

	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(300));
}

void TestFileUtils::testIgnoresSymlinks()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	// dir with symlink.
	QVERIFY(_createFileWithSize(tempDir.path() + "/real_file.txt", 100));
	QVERIFY(QFile::link(
		tempDir.path() + "/real_file.txt",
		tempDir.path() + "/symlink_to_file"
	));

	// symlink must be ignored.
	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(100));
}

void TestFileUtils::testNonExistentDirectory()
{
	// test behavior with non existent dir.
	QCOMPARE(FileUtils::calculateDirSize("/nonexistent/path/12345"), qint64(0));
}

void TestFileUtils::testMixedContent()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	// Complex struct is:
	// root/
	//		file1.txt (100)
	//		.hidden (50)
	//		dir1/
	//			file2.txt (200)
	//			symlink -> file2.txt (ignored)
	//		dir2/
	//			dir3/
	//				file3.txt (300)
	//		symlink_dir -> dir1 (ignored)

	QVERIFY(_createFileWithSize(tempDir.path() + "/file1.txt", 100));
	QVERIFY(_createFileWithSize(tempDir.path() + "/.hidden", 50));
	QVERIFY(QDir(tempDir.path()).mkpath("dir1"));
	QVERIFY(_createFileWithSize(tempDir.path() + "/dir1/file2.txt", 200));
	QVERIFY(QFile::link(
		tempDir.path() + "/dir1/file2.txt",
		tempDir.path() + "/dir1/symlink_file"
	));
	QVERIFY(QDir(tempDir.path()).mkpath("dir2/dir3"));
	QVERIFY(_createFileWithSize(tempDir.path() + "/dir2/dir3/file3.txt", 300));
	QVERIFY(QFile::link(
		tempDir.path() + "/dir1",
		tempDir.path() + "/symlink_dir"
	));

	// Expecting: 100 + 50 + 200 + 300 = 650 (symlink ignored)
	QCOMPARE(FileUtils::calculateDirSize(tempDir.path()), qint64(650));
}

QTEST_MAIN(TestFileUtils)
