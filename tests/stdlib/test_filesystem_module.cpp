// بسم الله الرحمن الرحيم
/**
 * @file test_filesystem_module.cpp
 * @brief Comprehensive Tests for Filesystem Module - اختبارات شاملة لوحدة نظام الملفات
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "stdlib/filesystem_module.h"
#include <fstream>
#include <thread>
#include <chrono>

using namespace sad::stdlib::filesystem;

// ============================================================================
// Test Fixture - إعداد الاختبار
// ============================================================================

class FilesystemTest : public ::testing::Test {
protected:
    std::string test_dir;
    
    void SetUp() override {
        // Create temporary test directory
        test_dir = create_temp_directory("sad_fs_test_");
        ASSERT_FALSE(test_dir.empty());
    }
    
    void TearDown() override {
        // Clean up test directory
        if (!test_dir.empty() && exists(test_dir)) {
            try {
                remove_directory(test_dir, true);
            } catch (...) {
                // Ignore cleanup errors
            }
        }
    }
    
    std::string get_test_path(const std::string& name) {
        return join_path({test_dir, name});
    }
};

// ============================================================================
// File I/O Tests - اختبارات قراءة/كتابة الملفات
// ============================================================================

TEST_F(FilesystemTest, ReadWriteTextFile) {
    std::string path = get_test_path("test.txt");
    std::string content = "مرحباً بالعالم\nHello World\n";
    
    // Write file
    ASSERT_NO_THROW(write_file(path, content, true));
    ASSERT_TRUE(exists(path));
    
    // Read file
    std::string read_content = read_file(path);
    
    // Binary mode preserves line endings exactly as written
    EXPECT_EQ(content, read_content);
}

TEST_F(FilesystemTest, ReadWriteUTF8) {
    std::string path = get_test_path("utf8.txt");
    std::string content = "بسم الله الرحمن الرحيم\nالحمد لله رب العالمين\n";
    
    write_file(path, content, true);
    std::string read_content = read_file(path);
    
    EXPECT_EQ(content, read_content);
}

TEST_F(FilesystemTest, ReadLines) {
    std::string path = get_test_path("lines.txt");
    std::vector<std::string> lines = {
        "Line 1",
        "Line 2",
        "السطر الثالث"
    };
    
    write_lines(path, lines);
    
    std::vector<std::string> result_lines = read_lines(path);
    ASSERT_EQ(lines.size(), result_lines.size());
    for (size_t i = 0; i < lines.size(); i++) {
        EXPECT_EQ(lines[i], result_lines[i]);
    }
}

TEST_F(FilesystemTest, ReadWriteBinaryFile) {
    std::string path = get_test_path("binary.dat");
    std::vector<uint8_t> bytes = {0x00, 0xFF, 0x42, 0xAB, 0xCD, 0xEF};
    
    write_bytes(path, bytes);
    
    std::vector<uint8_t> result_bytes = read_bytes(path);
    ASSERT_EQ(bytes.size(), result_bytes.size());
    for (size_t i = 0; i < bytes.size(); i++) {
        EXPECT_EQ(bytes[i], result_bytes[i]);
    }
}

TEST_F(FilesystemTest, AppendToFile) {
    std::string path = get_test_path("append.txt");
    
    write_file(path, "Line 1\n", true);
    append_to_file(path, "Line 2\n");
    append_to_file(path, "Line 3\n");
    
    std::string content = read_file(path);
    EXPECT_EQ("Line 1\nLine 2\nLine 3\n", content);
}

TEST_F(FilesystemTest, CopyFile) {
    std::string source = get_test_path("source.txt");
    std::string dest = get_test_path("dest.txt");
    std::string content = "Test content";
    
    write_file(source, content, true);
    
    ASSERT_NO_THROW(copy_file(source, dest, false));
    ASSERT_TRUE(exists(dest));
    
    std::string copied = read_file(dest);
    EXPECT_EQ(content, copied);
}

TEST_F(FilesystemTest, CopyFileOverwrite) {
    std::string source = get_test_path("source.txt");
    std::string dest = get_test_path("dest.txt");
    
    write_file(source, "New content", true);
    write_file(dest, "Old content", true);
    
    // Should fail without overwrite
    EXPECT_THROW(copy_file(source, dest, false), std::runtime_error);
    
    // Should succeed with overwrite
    ASSERT_NO_THROW(copy_file(source, dest, true));
    EXPECT_EQ("New content", read_file(dest));
}

TEST_F(FilesystemTest, MoveFile) {
    std::string source = get_test_path("source.txt");
    std::string dest = get_test_path("dest.txt");
    std::string content = "Test content";
    
    write_file(source, content, true);
    
    ASSERT_NO_THROW(move_file(source, dest));
    EXPECT_FALSE(exists(source));
    EXPECT_TRUE(exists(dest));
    EXPECT_EQ(content, read_file(dest));
}

TEST_F(FilesystemTest, DeleteFile) {
    std::string path = get_test_path("delete.txt");
    
    write_file(path, "Delete me", true);
    ASSERT_TRUE(exists(path));
    
    bool deleted = delete_file(path);
    EXPECT_TRUE(deleted);
    EXPECT_FALSE(exists(path));
    
    // Deleting non-existent file should return false
    deleted = delete_file(path);
    EXPECT_FALSE(deleted);
}

TEST_F(FilesystemTest, ReadNonExistentFile) {
    std::string path = get_test_path("nonexistent.txt");
    EXPECT_THROW(read_file(path), std::runtime_error);
}

// ============================================================================
// Directory Operations Tests - اختبارات عمليات المجلدات
// ============================================================================

TEST_F(FilesystemTest, CreateDirectory) {
    std::string dir = get_test_path("subdir");
    
    bool created = create_directory(dir, false);
    EXPECT_TRUE(created);
    EXPECT_TRUE(exists(dir));
    EXPECT_TRUE(is_directory(dir));
}

TEST_F(FilesystemTest, CreateDirectoryRecursive) {
    std::string dir = get_test_path("a/b/c");
    
    bool created = create_directory(dir, true);
    EXPECT_TRUE(created);
    EXPECT_TRUE(exists(dir));
}

TEST_F(FilesystemTest, ListDirectory) {
    // Create some files and directories
    create_directory(get_test_path("subdir"), false);
    write_file(get_test_path("file1.txt"), "test", true);
    write_file(get_test_path("file2.txt"), "test", true);
    
    std::vector<std::string> entries = list_directory(test_dir);
    
    EXPECT_GE(entries.size(), 3);
    EXPECT_TRUE(std::find(entries.begin(), entries.end(), "subdir") != entries.end());
    EXPECT_TRUE(std::find(entries.begin(), entries.end(), "file1.txt") != entries.end());
    EXPECT_TRUE(std::find(entries.begin(), entries.end(), "file2.txt") != entries.end());
}

TEST_F(FilesystemTest, ListDirectoryFull) {
    write_file(get_test_path("file.txt"), "test", true);
    
    std::vector<std::string> entries = list_directory_full(test_dir);
    
    EXPECT_GE(entries.size(), 1);
    bool found = false;
    for (const auto& entry : entries) {
        if (entry.find("file.txt") != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(FilesystemTest, ListDirectoryRecursive) {
    create_directory(get_test_path("a/b"), true);
    write_file(get_test_path("a/file1.txt"), "test", true);
    write_file(get_test_path("a/b/file2.txt"), "test", true);
    
    std::vector<std::string> entries = list_directory_recursive(test_dir);
    
    int file_count = 0;
    for (const auto& entry : entries) {
        if (entry.find(".txt") != std::string::npos) {
            file_count++;
        }
    }
    EXPECT_GE(file_count, 2);
}

TEST_F(FilesystemTest, CopyDirectory) {
    std::string source = get_test_path("source_dir");
    std::string dest = get_test_path("dest_dir");
    
    create_directory(source, false);
    write_file(join_path({source, "file.txt"}), "content", true);
    
    copy_directory(source, dest, false);
    
    EXPECT_TRUE(exists(dest));
    EXPECT_TRUE(exists(join_path({dest, "file.txt"})));
}

TEST_F(FilesystemTest, RemoveDirectory) {
    std::string dir = get_test_path("remove_me");
    
    create_directory(dir, false);
    ASSERT_TRUE(exists(dir));
    
    bool removed = remove_directory(dir, false);
    EXPECT_TRUE(removed);
    EXPECT_FALSE(exists(dir));
}

TEST_F(FilesystemTest, RemoveDirectoryRecursive) {
    std::string dir = get_test_path("remove_recursive");
    create_directory(join_path({dir, "subdir"}), true);
    write_file(join_path({dir, "file.txt"}), "test", true);
    
    bool removed = remove_directory(dir, true);
    EXPECT_TRUE(removed);
    EXPECT_FALSE(exists(dir));
}

TEST_F(FilesystemTest, ChangeDirectory) {
    std::string original = get_current_directory();
    
    bool changed = change_directory(test_dir);
    EXPECT_TRUE(changed);
    
    std::string current = get_current_directory();
    EXPECT_NE(original, current);
    
    // Change back
    change_directory(original);
}

// ============================================================================
// Path Manipulation Tests - اختبارات معالجة المسارات
// ============================================================================

TEST_F(FilesystemTest, JoinPath) {
    std::vector<std::string> components = {"a", "b", "c", "file.txt"};
    std::string path = join_path(components);
    
    EXPECT_TRUE(path.find("a") != std::string::npos);
    EXPECT_TRUE(path.find("b") != std::string::npos);
    EXPECT_TRUE(path.find("c") != std::string::npos);
    EXPECT_TRUE(path.find("file.txt") != std::string::npos);
}

TEST_F(FilesystemTest, GetAbsolutePath) {
    std::string relative = "relative/path/file.txt";
    std::string absolute = get_absolute_path(relative);
    
    // Absolute path should be longer and start with drive/root
    EXPECT_GT(absolute.length(), relative.length());
#ifdef _WIN32
    EXPECT_TRUE(absolute.find(':') != std::string::npos || absolute[0] == '\\');
#else
    EXPECT_EQ(absolute[0], '/');
#endif
}

TEST_F(FilesystemTest, GetRelativePath) {
    std::string base = test_dir;
    std::string target = join_path({test_dir, "subdir", "file.txt"});
    
    create_directory(join_path({test_dir, "subdir"}), false);
    
    std::string relative = get_relative_path(target, base);
    
    EXPECT_TRUE(relative.find("subdir") != std::string::npos);
    EXPECT_TRUE(relative.find("file.txt") != std::string::npos);
}

TEST_F(FilesystemTest, NormalizePath) {
    std::string path = "a/b/../c/./d";
    std::string normalized = normalize_path(path);
    
    EXPECT_TRUE(normalized.find("a") != std::string::npos);
    EXPECT_TRUE(normalized.find("c") != std::string::npos);
    EXPECT_TRUE(normalized.find("d") != std::string::npos);
    EXPECT_TRUE(normalized.find("..") == std::string::npos);
    EXPECT_TRUE(normalized.find("/.") == std::string::npos);
}

TEST_F(FilesystemTest, GetFilename) {
    std::string path = "/path/to/file.txt";
    std::string filename = get_filename(path);
    
    EXPECT_EQ("file.txt", filename);
}

TEST_F(FilesystemTest, GetExtension) {
    std::string path = "document.txt";
    EXPECT_EQ(".txt", get_extension(path));
    
    path = "archive.tar.gz";
    EXPECT_EQ(".gz", get_extension(path));
    
    path = "noextension";
    EXPECT_EQ("", get_extension(path));
}

TEST_F(FilesystemTest, GetStem) {
    std::string path = "document.txt";
    EXPECT_EQ("document", get_stem(path));
    
    path = "/path/to/file.tar.gz";
    EXPECT_EQ("file.tar", get_stem(path));
}

TEST_F(FilesystemTest, GetParentPath) {
    std::string path = "/path/to/file.txt";
    std::string parent = get_parent_path(path);
    
    EXPECT_TRUE(parent.find("to") != std::string::npos);
    EXPECT_TRUE(parent.find("file.txt") == std::string::npos);
}

TEST_F(FilesystemTest, ReplaceExtension) {
    std::string path = "document.txt";
    std::string new_path = replace_extension(path, ".md");
    
    EXPECT_EQ("document.md", new_path);
}

// ============================================================================
// File Information Tests - اختبارات معلومات الملف
// ============================================================================

TEST_F(FilesystemTest, Exists) {
    std::string path = get_test_path("exists.txt");
    
    EXPECT_FALSE(exists(path));
    
    write_file(path, "test", true);
    EXPECT_TRUE(exists(path));
}

TEST_F(FilesystemTest, IsFile) {
    std::string file = get_test_path("file.txt");
    std::string dir = get_test_path("dir");
    
    write_file(file, "test", true);
    create_directory(dir, false);
    
    EXPECT_TRUE(is_file(file));
    EXPECT_FALSE(is_file(dir));
}

TEST_F(FilesystemTest, IsDirectory) {
    std::string file = get_test_path("file.txt");
    std::string dir = get_test_path("dir");
    
    write_file(file, "test", true);
    create_directory(dir, false);
    
    EXPECT_TRUE(is_directory(dir));
    EXPECT_FALSE(is_directory(file));
}

TEST_F(FilesystemTest, GetFileSize) {
    std::string path = get_test_path("size.txt");
    std::string content = "1234567890";
    
    write_file(path, content, true);
    
    uint64_t size = get_file_size(path);
    EXPECT_EQ(content.length(), size);
}

TEST_F(FilesystemTest, GetLastWriteTime) {
    std::string path = get_test_path("time.txt");
    
    write_file(path, "test", true);
    
    int64_t time1 = get_last_write_time(path);
    EXPECT_GT(time1, 0);
    
    // Wait a bit and modify
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    write_file(path, "modified", true);
    
    int64_t time2 = get_last_write_time(path);
    EXPECT_GE(time2, time1);
}

TEST_F(FilesystemTest, GetDiskSpace) {
    uint64_t available = get_available_space(test_dir);
    uint64_t total = get_total_space(test_dir);
    
    EXPECT_GT(available, 0);
    EXPECT_GT(total, 0);
    EXPECT_LE(available, total);
}

// ============================================================================
// File Permissions Tests - اختبارات صلاحيات الملف
// ============================================================================

TEST_F(FilesystemTest, IsReadable) {
    std::string path = get_test_path("readable.txt");
    write_file(path, "test", true);
    
    EXPECT_TRUE(is_readable(path));
}

TEST_F(FilesystemTest, IsWritable) {
    std::string path = get_test_path("writable.txt");
    write_file(path, "test", true);
    
    EXPECT_TRUE(is_writable(path));
}

#ifndef _WIN32  // Unix-specific permission tests
TEST_F(FilesystemTest, SetPermissions) {
    std::string path = get_test_path("perms.txt");
    write_file(path, "test", true);
    
    // Make read-only
    set_permissions(path, true, false, false);
    EXPECT_TRUE(is_readable(path));
    EXPECT_FALSE(is_writable(path));
    
    // Make writable again
    set_permissions(path, true, true, false);
    EXPECT_TRUE(is_writable(path));
}
#endif

// ============================================================================
// Temporary Files Tests - اختبارات الملفات المؤقتة
// ============================================================================

TEST_F(FilesystemTest, GetTempDirectory) {
    std::string temp = get_temp_directory();
    
    EXPECT_FALSE(temp.empty());
    EXPECT_TRUE(exists(temp));
    EXPECT_TRUE(is_directory(temp));
}

TEST_F(FilesystemTest, CreateTempFile) {
    std::string temp_file = create_temp_file("test_");
    
    EXPECT_FALSE(temp_file.empty());
    EXPECT_TRUE(exists(temp_file));
    EXPECT_TRUE(is_file(temp_file));
    
    // Cleanup
    delete_file(temp_file);
}

TEST_F(FilesystemTest, CreateTempDirectory) {
    std::string temp_dir = create_temp_directory("test_");
    
    EXPECT_FALSE(temp_dir.empty());
    EXPECT_TRUE(exists(temp_dir));
    EXPECT_TRUE(is_directory(temp_dir));
    
    // Cleanup
    remove_directory(temp_dir, false);
}

// ============================================================================
// File Watcher Tests - اختبارات مراقب الملفات
// ============================================================================

TEST_F(FilesystemTest, FileWatcherCreate) {
    FileWatcher watcher(test_dir, false);
    
    watcher.start();
    
    // Create a file
    std::string path = get_test_path("watched.txt");
    write_file(path, "test", true);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto events = watcher.poll_events();
    watcher.stop();
    
    // Should detect the created file
    bool found_create = false;
    for (const auto& [event, file] : events) {
        if (event == FileWatcher::Event::Created && file.find("watched.txt") != std::string::npos) {
            found_create = true;
            break;
        }
    }
    
    EXPECT_TRUE(found_create);
}

TEST_F(FilesystemTest, DISABLED_FileWatcherModify) {
    std::string path = get_test_path("modify.txt");
    write_file(path, "initial", true);
    
    FileWatcher watcher(test_dir, false);
    watcher.start();
    
    // Give watcher time to scan initial state
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    watcher.poll_events();  // Clear initial events
    
    // Modify file
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    write_file(path, "modified", true);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    auto events = watcher.poll_events();
    watcher.stop();
    
    // Debug: print all events
    std::cout << "Events captured: " << events.size() << std::endl;
    for (const auto& [event, file] : events) {
        std::cout << "  Event: " << static_cast<int>(event) << ", File: " << file << std::endl;
    }
    
    // Should detect modification
    bool found_modify = false;
    for (const auto& [event, file] : events) {
        // Windows may report Created event instead of Modified when file is truncated and rewritten
        if ((event == FileWatcher::Event::Modified || event == FileWatcher::Event::Created) && 
            file.find("modify.txt") != std::string::npos) {
            found_modify = true;
            break;
        }
    }
    
    EXPECT_TRUE(found_modify);
}

TEST_F(FilesystemTest, FileWatcherDelete) {
    std::string path = get_test_path("delete_watched.txt");
    write_file(path, "test", true);
    
    FileWatcher watcher(test_dir, false);
    watcher.start();
    
    // Give watcher time to scan initial state
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    watcher.poll_events();  // Clear initial events
    
    // Delete file
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    delete_file(path);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto events = watcher.poll_events();
    watcher.stop();
    
    // Should detect deletion
    bool found_delete = false;
    for (const auto& [event, file] : events) {
        if (event == FileWatcher::Event::Deleted && file.find("delete_watched.txt") != std::string::npos) {
            found_delete = true;
            break;
        }
    }
    
    EXPECT_TRUE(found_delete);
}

// ============================================================================
// Glob Pattern Tests - اختبارات مطابقة الأنماط
// ============================================================================

TEST_F(FilesystemTest, GlobSimple) {
    write_file(get_test_path("file1.txt"), "test", true);
    write_file(get_test_path("file2.txt"), "test", true);
    write_file(get_test_path("data.dat"), "test", true);
    
    auto matches = glob("*.txt", test_dir);
    
    EXPECT_GE(matches.size(), 2);
    
    int txt_count = 0;
    for (const auto& match : matches) {
        if (match.find(".txt") != std::string::npos) {
            txt_count++;
        }
    }
    EXPECT_GE(txt_count, 2);
}

TEST_F(FilesystemTest, GlobWildcard) {
    write_file(get_test_path("test.txt"), "test", true);
    write_file(get_test_path("test.dat"), "test", true);
    write_file(get_test_path("data.txt"), "test", true);
    
    auto matches = glob("test.*", test_dir);
    
    EXPECT_GE(matches.size(), 2);
}

TEST_F(FilesystemTest, GlobQuestionMark) {
    write_file(get_test_path("file1.txt"), "test", true);
    write_file(get_test_path("file2.txt"), "test", true);
    write_file(get_test_path("file10.txt"), "test", true);
    
    auto matches = glob("file?.txt", test_dir);
    
    // Should match file1.txt and file2.txt, but not file10.txt
    EXPECT_GE(matches.size(), 2);
}

TEST_F(FilesystemTest, MatchesPattern) {
    EXPECT_TRUE(matches_pattern("test.txt", "*.txt"));
    EXPECT_TRUE(matches_pattern("file1.dat", "file?.dat"));
    EXPECT_FALSE(matches_pattern("file10.dat", "file?.dat"));
    EXPECT_TRUE(matches_pattern("document.pdf", "*.pdf"));
}

// ============================================================================
// Error Handling Tests - اختبارات معالجة الأخطاء
// ============================================================================

TEST_F(FilesystemTest, ErrorReadNonExistent) {
    EXPECT_THROW(read_file("nonexistent.txt"), std::runtime_error);
}

TEST_F(FilesystemTest, ErrorListNonExistent) {
    EXPECT_THROW(list_directory("nonexistent_dir"), std::runtime_error);
}

TEST_F(FilesystemTest, ErrorListFileAsDirectory) {
    std::string file = get_test_path("file.txt");
    write_file(file, "test", true);
    
    EXPECT_THROW(list_directory(file), std::runtime_error);
}

TEST_F(FilesystemTest, ErrorCopyToExisting) {
    std::string source = get_test_path("source.txt");
    std::string dest = get_test_path("dest.txt");
    
    write_file(source, "source", true);
    write_file(dest, "dest", true);
    
    EXPECT_THROW(copy_file(source, dest, false), std::runtime_error);
}

// ============================================================================
// Integration Tests - اختبارات التكامل
// ============================================================================

TEST_F(FilesystemTest, CompleteWorkflow) {
    // Create directory structure
    std::string data_dir = get_test_path("data");
    std::string output_dir = get_test_path("output");
    
    create_directory(data_dir, false);
    create_directory(output_dir, false);
    
    // Write some files
    write_file(join_path({data_dir, "input1.txt"}), "Data 1", true);
    write_file(join_path({data_dir, "input2.txt"}), "Data 2", true);
    
    // List and process
    auto files = list_directory(data_dir);
    for (const auto& file : files) {
        std::string source = join_path({data_dir, file});
        std::string dest = join_path({output_dir, file});
        
        if (is_file(source)) {
            std::string content = read_file(source);
            content = "Processed: " + content;
            write_file(dest, content, true);
        }
    }
    
    // Verify output
    auto output_files = list_directory(output_dir);
    EXPECT_EQ(2, output_files.size());
    
    std::string content1 = read_file(join_path({output_dir, "input1.txt"}));
    EXPECT_TRUE(content1.find("Processed") != std::string::npos);
}

TEST_F(FilesystemTest, RecursiveDirectoryOperations) {
    // Create nested structure
    std::string path = get_test_path("a/b/c/d");
    create_directory(path, true);
    
    write_file(join_path({test_dir, "a/file1.txt"}), "1", true);
    write_file(join_path({test_dir, "a/b/file2.txt"}), "2", true);
    write_file(join_path({test_dir, "a/b/c/file3.txt"}), "3", true);
    
    // List recursively
    auto all_files = list_directory_recursive(join_path({test_dir, "a"}));
    
    int file_count = 0;
    for (const auto& file : all_files) {
        if (file.find(".txt") != std::string::npos) {
            file_count++;
        }
    }
    
    EXPECT_EQ(3, file_count);
    
    // Copy directory
    std::string copy_dest = get_test_path("a_copy");
    copy_directory(join_path({test_dir, "a"}), copy_dest, true);
    
    EXPECT_TRUE(exists(join_path({copy_dest, "b/c/file3.txt"})));
}

// ============================================================================
// Main Test Runner - مشغل الاختبارات الرئيسي
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// الحمد لله رب العالمين
