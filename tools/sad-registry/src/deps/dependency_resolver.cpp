// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: dependency_resolver.cpp
 * الوصف: تنفيذ محرك حل التبعيات
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "dependency_resolver.h"
#include "../storage/database.h"

#include <algorithm>
#include <sstream>
#include <regex>
#include <chrono>
#include <queue>
#include <stack>

namespace sad {
namespace registry {
namespace deps {

// ═══════════════════════════════════════════════════════════════════════════════
// Version Implementation
// ═══════════════════════════════════════════════════════════════════════════════

Version::Version(const std::string& str) {
    // Parse: MAJOR.MINOR.PATCH[-prerelease][+build]
    std::regex semver_regex(R"((\d+)\.(\d+)\.(\d+)(?:-([a-zA-Z0-9.-]+))?(?:\+([a-zA-Z0-9.-]+))?)");
    std::smatch match;
    
    if (std::regex_match(str, match, semver_regex)) {
        major = std::stoi(match[1].str());
        minor = std::stoi(match[2].str());
        patch = std::stoi(match[3].str());
        if (match[4].matched) prerelease = match[4].str();
        if (match[5].matched) build_metadata = match[5].str();
    }
}

std::string Version::to_string() const {
    std::ostringstream oss;
    oss << major << "." << minor << "." << patch;
    if (!prerelease.empty()) oss << "-" << prerelease;
    if (!build_metadata.empty()) oss << "+" << build_metadata;
    return oss.str();
}

bool Version::operator==(const Version& other) const {
    // Note: build metadata is ignored per SemVer spec
    return major == other.major &&
           minor == other.minor &&
           patch == other.patch &&
           prerelease == other.prerelease;
}

bool Version::operator<(const Version& other) const {
    // Compare major.minor.patch
    if (major != other.major) return major < other.major;
    if (minor != other.minor) return minor < other.minor;
    if (patch != other.patch) return patch < other.patch;
    
    // Prerelease has lower precedence than release
    if (prerelease.empty() && !other.prerelease.empty()) return false;
    if (!prerelease.empty() && other.prerelease.empty()) return true;
    
    // Compare prerelease lexicographically
    return prerelease < other.prerelease;
}

// ═══════════════════════════════════════════════════════════════════════════════
// VersionConstraint Implementation
// ═══════════════════════════════════════════════════════════════════════════════

VersionConstraint VersionConstraint::parse(const std::string& str) {
    VersionConstraint constraint;
    std::string s = str;
    
    // Trim whitespace
    while (!s.empty() && std::isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && std::isspace(s.back())) s.pop_back();
    
    if (s.empty() || s == "*") {
        constraint.type = ConstraintType::Any;
        return constraint;
    }
    
    // Check for caret (^)
    if (s[0] == '^') {
        constraint.type = ConstraintType::Caret;
        constraint.min_version = Version(s.substr(1));
        // ^1.2.3 means >=1.2.3 <2.0.0 (or <1.3.0 if major=0)
        if (constraint.min_version.major == 0) {
            if (constraint.min_version.minor == 0) {
                constraint.max_version = constraint.min_version.next_patch();
            } else {
                constraint.max_version = Version(0, constraint.min_version.minor + 1, 0);
            }
        } else {
            constraint.max_version = constraint.min_version.next_major();
        }
        constraint.min_inclusive = true;
        constraint.max_inclusive = false;
        return constraint;
    }
    
    // Check for tilde (~)
    if (s[0] == '~') {
        constraint.type = ConstraintType::Tilde;
        constraint.min_version = Version(s.substr(1));
        // ~1.2.3 means >=1.2.3 <1.3.0
        constraint.max_version = Version(constraint.min_version.major,
                                         constraint.min_version.minor + 1, 0);
        constraint.min_inclusive = true;
        constraint.max_inclusive = false;
        return constraint;
    }
    
    // Check for range (e.g., ">=1.0.0 <2.0.0")
    if (s.find(' ') != std::string::npos) {
        constraint.type = ConstraintType::Range;
        std::istringstream iss(s);
        std::string part;
        bool first = true;
        
        while (iss >> part) {
            if (part.substr(0, 2) == ">=") {
                constraint.min_version = Version(part.substr(2));
                constraint.min_inclusive = true;
            } else if (part.substr(0, 1) == ">") {
                constraint.min_version = Version(part.substr(1));
                constraint.min_inclusive = false;
            } else if (part.substr(0, 2) == "<=") {
                constraint.max_version = Version(part.substr(2));
                constraint.max_inclusive = true;
            } else if (part.substr(0, 1) == "<") {
                constraint.max_version = Version(part.substr(1));
                constraint.max_inclusive = false;
            }
            first = false;
        }
        return constraint;
    }
    
    // Check for operators
    if (s.substr(0, 2) == ">=") {
        constraint.type = ConstraintType::GreaterEq;
        constraint.min_version = Version(s.substr(2));
        constraint.min_inclusive = true;
        return constraint;
    }
    if (s.substr(0, 2) == "<=") {
        constraint.type = ConstraintType::LessEq;
        constraint.max_version = Version(s.substr(2));
        constraint.max_inclusive = true;
        return constraint;
    }
    if (s.substr(0, 1) == ">") {
        constraint.type = ConstraintType::Greater;
        constraint.min_version = Version(s.substr(1));
        constraint.min_inclusive = false;
        return constraint;
    }
    if (s.substr(0, 1) == "<") {
        constraint.type = ConstraintType::Less;
        constraint.max_version = Version(s.substr(1));
        constraint.max_inclusive = false;
        return constraint;
    }
    if (s.substr(0, 1) == "=") {
        constraint.type = ConstraintType::Exact;
        constraint.min_version = Version(s.substr(1));
        constraint.max_version = constraint.min_version;
        constraint.min_inclusive = true;
        constraint.max_inclusive = true;
        return constraint;
    }
    
    // Check for wildcard (*.*.*, 1.*, 1.2.*)
    if (s.find('*') != std::string::npos) {
        constraint.type = ConstraintType::Wildcard;
        // Parse partial version
        std::regex wildcard_regex(R"((\d+)?\.?(\d+)?\.?\*)");
        std::smatch match;
        if (std::regex_match(s, match, wildcard_regex)) {
            if (match[1].matched && match[2].matched) {
                // 1.2.*
                int maj = std::stoi(match[1].str());
                int min = std::stoi(match[2].str());
                constraint.min_version = Version(maj, min, 0);
                constraint.max_version = Version(maj, min + 1, 0);
            } else if (match[1].matched) {
                // 1.*
                int maj = std::stoi(match[1].str());
                constraint.min_version = Version(maj, 0, 0);
                constraint.max_version = Version(maj + 1, 0, 0);
            } else {
                // *
                constraint.type = ConstraintType::Any;
            }
        }
        constraint.min_inclusive = true;
        constraint.max_inclusive = false;
        return constraint;
    }
    
    // Default: exact version
    constraint.type = ConstraintType::Exact;
    constraint.min_version = Version(s);
    constraint.max_version = constraint.min_version;
    constraint.min_inclusive = true;
    constraint.max_inclusive = true;
    return constraint;
}

bool VersionConstraint::satisfies(const Version& version) const {
    switch (type) {
        case ConstraintType::Any:
            return true;
            
        case ConstraintType::Exact:
            return version == min_version;
            
        case ConstraintType::Greater:
            return version > min_version;
            
        case ConstraintType::GreaterEq:
            return version >= min_version;
            
        case ConstraintType::Less:
            return version < max_version;
            
        case ConstraintType::LessEq:
            return version <= max_version;
            
        case ConstraintType::Caret:
        case ConstraintType::Tilde:
        case ConstraintType::Range:
        case ConstraintType::Wildcard: {
            bool min_ok = min_inclusive ? (version >= min_version) : (version > min_version);
            bool max_ok = max_inclusive ? (version <= max_version) : (version < max_version);
            return min_ok && max_ok;
        }
    }
    return false;
}

std::string VersionConstraint::to_string() const {
    switch (type) {
        case ConstraintType::Any: return "*";
        case ConstraintType::Exact: return min_version.to_string();
        case ConstraintType::Greater: return ">" + min_version.to_string();
        case ConstraintType::GreaterEq: return ">=" + min_version.to_string();
        case ConstraintType::Less: return "<" + max_version.to_string();
        case ConstraintType::LessEq: return "<=" + max_version.to_string();
        case ConstraintType::Caret: return "^" + min_version.to_string();
        case ConstraintType::Tilde: return "~" + min_version.to_string();
        case ConstraintType::Range:
        case ConstraintType::Wildcard:
            return (min_inclusive ? ">=" : ">") + min_version.to_string() + " " +
                   (max_inclusive ? "<=" : "<") + max_version.to_string();
    }
    return "*";
}

std::optional<VersionConstraint> VersionConstraint::intersect(const VersionConstraint& other) const {
    // Compute intersection of two constraints
    VersionConstraint result;
    result.type = ConstraintType::Range;
    
    // Determine min
    if (type == ConstraintType::Any) {
        result.min_version = other.min_version;
        result.min_inclusive = other.min_inclusive;
    } else if (other.type == ConstraintType::Any) {
        result.min_version = min_version;
        result.min_inclusive = min_inclusive;
    } else if (min_version > other.min_version) {
        result.min_version = min_version;
        result.min_inclusive = min_inclusive;
    } else if (other.min_version > min_version) {
        result.min_version = other.min_version;
        result.min_inclusive = other.min_inclusive;
    } else {
        result.min_version = min_version;
        result.min_inclusive = min_inclusive && other.min_inclusive;
    }
    
    // Determine max
    if (type == ConstraintType::Any || type == ConstraintType::Greater || type == ConstraintType::GreaterEq) {
        result.max_version = other.max_version;
        result.max_inclusive = other.max_inclusive;
    } else if (other.type == ConstraintType::Any || other.type == ConstraintType::Greater || other.type == ConstraintType::GreaterEq) {
        result.max_version = max_version;
        result.max_inclusive = max_inclusive;
    } else if (max_version < other.max_version) {
        result.max_version = max_version;
        result.max_inclusive = max_inclusive;
    } else if (other.max_version < max_version) {
        result.max_version = other.max_version;
        result.max_inclusive = other.max_inclusive;
    } else {
        result.max_version = max_version;
        result.max_inclusive = max_inclusive && other.max_inclusive;
    }
    
    // Check if valid
    if (result.min_version > result.max_version) {
        return std::nullopt;
    }
    if (result.min_version == result.max_version && 
        !(result.min_inclusive && result.max_inclusive)) {
        return std::nullopt;
    }
    
    return result;
}

bool VersionConstraint::is_empty() const {
    if (type == ConstraintType::Any) return false;
    if (type == ConstraintType::Exact || type == ConstraintType::Greater || 
        type == ConstraintType::GreaterEq) return false;
    
    // For ranges
    if (min_version > max_version) return true;
    if (min_version == max_version && !(min_inclusive && max_inclusive)) return true;
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DependencyResolver Implementation
// ═══════════════════════════════════════════════════════════════════════════════

DependencyResolver::DependencyResolver(Database& db) : db_(db) {}

ResolutionResult DependencyResolver::resolve(
    const std::vector<Dependency>& root_packages,
    bool include_dev,
    const std::string& platform) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    ResolutionResult result;
    
    if (root_packages.empty()) {
        result.success = true;
        return result;
    }
    
    // Check cache
    std::string cache_key = compute_cache_key(root_packages);
    if (use_cache_) {
        auto cached = get_cached_result(cache_key);
        if (cached) {
            cached->cache_hits = 1;
            return *cached;
        }
    }
    
    // Initialize state
    State state;
    
    // Resolve each root package
    for (const auto& dep : root_packages) {
        // Filter by dependency type
        if (dep.type == DependencyType::Dev && !include_dev) continue;
        
        // Filter by platform
        if (!dep.platform_filter.empty() && 
            dep.platform_filter != "all" && 
            dep.platform_filter != platform) continue;
        
        if (!resolve_recursive(state, dep)) {
            result.success = false;
            result.error_message = "فشل في حل تبعيات الحزمة: " + dep.package_name;
            
            // Build conflict info
            Conflict conflict;
            conflict.package_name = dep.package_name;
            conflict.reason = "لا يوجد إصدار يرضي جميع القيود";
            if (state.constraints.count(dep.package_name)) {
                conflict.conflicting_constraints = state.constraints[dep.package_name];
            }
            result.conflicts.push_back(conflict);
            
            auto end_time = std::chrono::high_resolution_clock::now();
            result.resolution_time_ms = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
            );
            return result;
        }
    }
    
    // Build result
    result.success = true;
    result.packages_considered = static_cast<int>(state.visited.size());
    
    // Build dependency graph for topological sort
    std::map<std::string, std::vector<std::string>> graph;
    for (const auto& [name, version] : state.selected) {
        auto info = get_package_info(name, version);
        for (const auto& dep : info.dependencies) {
            graph[name].push_back(dep.package_name);
        }
        if (graph.find(name) == graph.end()) {
            graph[name] = {};
        }
    }
    
    // Topological sort
    auto sorted = topological_sort(graph);
    
    // Build resolved packages list
    for (const auto& name : sorted) {
        if (state.selected.count(name)) {
            auto info = get_package_info(name, state.selected[name]);
            ResolvedPackage rp;
            rp.name = name;
            rp.version = state.selected[name];
            rp.checksum = info.checksum;
            for (const auto& d : info.dependencies) {
                rp.depends_on.push_back(d.package_name);
            }
            // Check if root
            for (const auto& root : root_packages) {
                if (root.package_name == name) {
                    rp.is_root = true;
                    rp.type = root.type;
                    break;
                }
            }
            result.packages.push_back(rp);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.resolution_time_ms = static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    // Cache result
    if (use_cache_) {
        cache_result(cache_key, result);
    }
    
    return result;
}

bool DependencyResolver::resolve_recursive(State& state, const Dependency& dep) {
    // Check depth limit
    if (state.depth > max_depth_) {
        return false;
    }
    
    const std::string& name = dep.package_name;
    
    // Check for cycles
    if (state.visited.count(name) && 
        std::find(state.path.begin(), state.path.end(), name) != state.path.end()) {
        // Cycle detected - might be OK if versions are compatible
        if (state.selected.count(name)) {
            return dep.constraint.satisfies(state.selected[name]);
        }
        return false;
    }
    
    // Add constraint
    state.constraints[name].push_back(dep.constraint);
    
    // If already selected, check compatibility
    if (state.selected.count(name)) {
        return dep.constraint.satisfies(state.selected[name]);
    }
    
    // Select best version
    auto version = select_best_version(name, state.constraints[name]);
    if (!version) {
        return false;
    }
    
    // Record selection
    state.selected[name] = *version;
    state.visited.insert(name);
    state.path.push_back(name);
    state.depth++;
    
    // Recursively resolve dependencies
    auto info = get_package_info(name, *version);
    for (const auto& subdep : info.dependencies) {
        if (subdep.optional) continue;
        
        if (!resolve_recursive(state, subdep)) {
            // Backtrack
            state.selected.erase(name);
            state.path.pop_back();
            state.depth--;
            return false;
        }
    }
    
    state.path.pop_back();
    state.depth--;
    return true;
}

std::optional<Version> DependencyResolver::select_best_version(
    const std::string& package,
    const std::vector<VersionConstraint>& constraints) {
    
    // Fetch available versions
    auto versions = fetch_versions(package);
    if (versions.empty()) {
        return std::nullopt;
    }
    
    // Filter by constraints
    std::vector<Version> candidates;
    for (const auto& info : versions) {
        if (info.yanked) continue;
        if (!allow_prereleases_ && info.version.is_prerelease()) continue;
        
        bool satisfies_all = true;
        for (const auto& constraint : constraints) {
            if (!constraint.satisfies(info.version)) {
                satisfies_all = false;
                break;
            }
        }
        
        if (satisfies_all) {
            candidates.push_back(info.version);
        }
    }
    
    if (candidates.empty()) {
        return std::nullopt;
    }
    
    // Sort descending (newest first) and pick the best
    std::sort(candidates.begin(), candidates.end(), std::greater<Version>());
    
    // Limit candidates to try
    if (static_cast<int>(candidates.size()) > max_versions_to_try_) {
        candidates.resize(max_versions_to_try_);
    }
    
    // Return newest compatible version
    return candidates[0];
}

PackageVersionInfo DependencyResolver::get_package_info(const std::string& name, const Version& version) {
    // Check cache
    if (version_cache_.count(name)) {
        for (const auto& info : version_cache_[name]) {
            if (info.version == version) {
                return info;
            }
        }
    }
    
    // Fetch from database
    auto pkg = db_.get_package(name);
    if (!pkg) {
        return PackageVersionInfo{name, version, {}, "", false};
    }
    
    auto ver = db_.get_version(pkg->id, version.to_string());
    if (!ver) {
        return PackageVersionInfo{name, version, {}, "", false};
    }
    
    PackageVersionInfo info;
    info.name = name;
    info.version = version;
    info.checksum = ver->checksum_sha256;
    info.yanked = ver->is_yanked;
    
    // Get dependencies
    auto deps_v2 = db_.get_dependencies_v2(ver->id);
    for (const auto& d : deps_v2) {
        Dependency dep;
        dep.package_name = d.dependency_name;
        dep.constraint = VersionConstraint::parse(d.version_constraint);
        dep.type = dependency_type_from_string(d.dependency_type);
        dep.optional = d.is_optional;
        dep.platform_filter = d.platform_filter;
        info.dependencies.push_back(dep);
    }
    
    return info;
}

std::vector<PackageVersionInfo> DependencyResolver::fetch_versions(const std::string& package_name) {
    // Check cache
    if (version_cache_.count(package_name)) {
        return version_cache_[package_name];
    }
    
    // Fetch from database
    auto pkg = db_.get_package(package_name);
    if (!pkg) {
        return {};
    }
    
    auto versions = db_.get_package_versions(pkg->id);
    std::vector<PackageVersionInfo> result;
    
    for (const auto& v : versions) {
        PackageVersionInfo info;
        info.name = package_name;
        info.version = Version(v.version);
        info.checksum = v.checksum_sha256;
        info.yanked = v.is_yanked;
        result.push_back(info);
    }
    
    // Cache
    version_cache_[package_name] = result;
    
    return result;
}

std::string DependencyResolver::compute_cache_key(const std::vector<Dependency>& deps) {
    std::ostringstream oss;
    for (const auto& d : deps) {
        oss << d.package_name << ":" << d.constraint.to_string() << ";";
    }
    return oss.str();
}

std::optional<ResolutionResult> DependencyResolver::get_cached_result(const std::string& key) {
    auto cached = db_.get_dependency_cache(key);
    if (!cached) return std::nullopt;
    
    // Parse cached result (would need JSON parsing in production)
    // For now, just return empty
    return std::nullopt;
}

void DependencyResolver::cache_result(const std::string& key, const ResolutionResult& result) {
    // Serialize result (would use JSON in production)
    std::ostringstream oss;
    for (const auto& p : result.packages) {
        oss << p.name << "=" << p.version.to_string() << ";";
    }
    db_.cache_dependency_resolution(key, oss.str(), result.resolution_time_ms);
}

ResolutionResult DependencyResolver::resolve_from_manifest(const std::string& manifest_path) {
    // Would parse sad.toml file
    // For now, return empty
    (void)manifest_path;
    ResolutionResult result;
    result.error_message = "Manifest parsing not implemented";
    return result;
}

bool DependencyResolver::check_compatibility(
    const std::string& package_name,
    const Version& version,
    const std::vector<VersionConstraint>& constraints) {
    
    (void)package_name;
    for (const auto& c : constraints) {
        if (!c.satisfies(version)) return false;
    }
    return true;
}

std::vector<Version> DependencyResolver::get_available_versions(
    const std::string& package_name,
    const VersionConstraint& constraint) {
    
    auto all_versions = fetch_versions(package_name);
    std::vector<Version> result;
    
    for (const auto& info : all_versions) {
        if (!info.yanked && constraint.satisfies(info.version)) {
            result.push_back(info.version);
        }
    }
    
    std::sort(result.begin(), result.end(), std::greater<Version>());
    return result;
}

std::vector<std::vector<std::string>> DependencyResolver::detect_cycles(
    const std::vector<Dependency>& dependencies) {
    
    std::vector<std::vector<std::string>> cycles;
    
    // Build graph
    std::map<std::string, std::vector<std::string>> graph;
    std::set<std::string> all_packages;
    
    for (const auto& dep : dependencies) {
        all_packages.insert(dep.package_name);
        auto info = get_package_info(dep.package_name, Version("0.0.0")); // Latest
        for (const auto& subdep : info.dependencies) {
            graph[dep.package_name].push_back(subdep.package_name);
            all_packages.insert(subdep.package_name);
        }
    }
    
    // DFS to find cycles
    std::set<std::string> visited;
    std::set<std::string> in_stack;
    std::vector<std::string> path;
    
    std::function<void(const std::string&)> dfs = [&](const std::string& node) {
        if (in_stack.count(node)) {
            // Found cycle
            std::vector<std::string> cycle;
            bool found = false;
            for (const auto& p : path) {
                if (p == node) found = true;
                if (found) cycle.push_back(p);
            }
            cycle.push_back(node);
            cycles.push_back(cycle);
            return;
        }
        
        if (visited.count(node)) return;
        
        visited.insert(node);
        in_stack.insert(node);
        path.push_back(node);
        
        for (const auto& next : graph[node]) {
            dfs(next);
        }
        
        path.pop_back();
        in_stack.erase(node);
    };
    
    for (const auto& pkg : all_packages) {
        if (!visited.count(pkg)) {
            dfs(pkg);
        }
    }
    
    return cycles;
}

void DependencyResolver::clear_cache() {
    version_cache_.clear();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

Version parse_version(const std::string& str) {
    return Version(str);
}

VersionConstraint parse_constraint(const std::string& str) {
    return VersionConstraint::parse(str);
}

std::vector<std::string> topological_sort(
    const std::map<std::string, std::vector<std::string>>& graph) {
    
    std::vector<std::string> result;
    std::set<std::string> visited;
    std::set<std::string> in_progress;
    
    std::function<bool(const std::string&)> visit = [&](const std::string& node) -> bool {
        if (visited.count(node)) return true;
        if (in_progress.count(node)) return false; // Cycle
        
        in_progress.insert(node);
        
        auto it = graph.find(node);
        if (it != graph.end()) {
            for (const auto& dep : it->second) {
                if (!visit(dep)) return false;
            }
        }
        
        in_progress.erase(node);
        visited.insert(node);
        result.push_back(node);
        return true;
    };
    
    for (const auto& [node, _] : graph) {
        visit(node);
    }
    
    return result;
}

DependencyType dependency_type_from_string(const std::string& str) {
    if (str == "dev" || str == "development") return DependencyType::Dev;
    if (str == "build") return DependencyType::Build;
    if (str == "optional") return DependencyType::Optional;
    if (str == "peer") return DependencyType::Peer;
    return DependencyType::Runtime;
}

std::string dependency_type_to_string(DependencyType type) {
    switch (type) {
        case DependencyType::Dev: return "dev";
        case DependencyType::Build: return "build";
        case DependencyType::Optional: return "optional";
        case DependencyType::Peer: return "peer";
        case DependencyType::Runtime: return "runtime";
    }
    return "runtime";
}

} // namespace deps
} // namespace registry
} // namespace sad
