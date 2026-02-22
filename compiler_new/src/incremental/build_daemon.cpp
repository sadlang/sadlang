// Build Daemon — بناء تزايدي في الخلفية
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>

namespace sadc {
namespace incremental {

struct FileNode {
    std::string path;
    std::string hash;
    std::chrono::system_clock::time_point lastModified;
    std::vector<std::string> dependencies;
    bool needsRecompile = false;
};

class DependencyTracker {
    std::unordered_map<std::string, FileNode> files;
    std::unordered_map<std::string, std::vector<std::string>> reverseDeps;

public:
    void addFile(const std::string& path, const std::string& hash) {
        files[path] = {path, hash, std::chrono::system_clock::now(), {}, false};
    }

    void addDependency(const std::string& from, const std::string& to) {
        files[from].dependencies.push_back(to);
        reverseDeps[to].push_back(from);
    }

    std::vector<std::string> getAffectedFiles(const std::string& changedFile) {
        std::vector<std::string> affected;
        std::unordered_set<std::string> visited;
        propagateChange(changedFile, affected, visited);
        return affected;
    }

private:
    void propagateChange(const std::string& file, std::vector<std::string>& affected,
                         std::unordered_set<std::string>& visited) {
        if (visited.count(file)) return;
        visited.insert(file);
        affected.push_back(file);
        auto it = reverseDeps.find(file);
        if (it != reverseDeps.end()) {
            for (const auto& dep : it->second) {
                propagateChange(dep, affected, visited);
            }
        }
    }
};

class BuildDaemon {
    DependencyTracker tracker;
    bool running = false;
    std::function<void(const std::string&, float)> progressCallback;
    std::function<void(const std::string&, int, const std::string&)> errorCallback;

public:
    void start() { running = true; }
    void stop() { running = false; }
    bool isRunning() const { return running; }

    void setProgressCallback(std::function<void(const std::string&, float)> cb) {
        progressCallback = std::move(cb);
    }

    void setErrorCallback(std::function<void(const std::string&, int, const std::string&)> cb) {
        errorCallback = std::move(cb);
    }

    void onFileChanged(const std::string& path, const std::string& newHash) {
        if (!running) return;
        auto affected = tracker.getAffectedFiles(path);
        float total = static_cast<float>(affected.size());
        float done = 0;
        for (const auto& file : affected) {
            if (progressCallback) {
                progressCallback(file, done / total);
            }
            // Compile file
            done += 1.0f;
        }
        if (progressCallback) {
            progressCallback("", 1.0f);
        }
    }

    void fullBuild(const std::vector<std::string>& files) {
        float total = static_cast<float>(files.size());
        float done = 0;
        for (const auto& file : files) {
            if (progressCallback) {
                progressCallback(file, done / total);
            }
            tracker.addFile(file, "");
            done += 1.0f;
        }
    }
};

// Predictive compilation — ترجمة تنبؤية
class PredictiveCompiler {
    std::vector<std::string> recentlyEdited;
    std::unordered_map<std::string, int> editFrequency;

public:
    void recordEdit(const std::string& file) {
        editFrequency[file]++;
        recentlyEdited.push_back(file);
        if (recentlyEdited.size() > 100) recentlyEdited.erase(recentlyEdited.begin());
    }

    std::vector<std::string> predictNextFiles() {
        // Files frequently edited after recent files are likely to be edited next
        std::vector<std::pair<std::string, int>> ranked(editFrequency.begin(), editFrequency.end());
        std::sort(ranked.begin(), ranked.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        std::vector<std::string> result;
        for (size_t i = 0; i < std::min(size_t(5), ranked.size()); i++) {
            result.push_back(ranked[i].first);
        }
        return result;
    }
};

} // namespace incremental
} // namespace sadc
