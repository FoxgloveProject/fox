#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
//Added this include
#include "CLI/CLI.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Package database structure
struct Package {
    std::string name;
    std::string version;
    std::string description;
    std::vector<std::string> dependencies;
    std::string maintainer;
    std::string license;
    bool installed = false;
};

// Global package database (in a real implementation, this would be loaded from files)
std::map<std::string, Package> package_database;
std::set<std::string> installed_packages;

// Helper function declarations
void initialize_package_database();
bool check_dependencies(const std::vector<std::string>& dependencies);
bool download_package(const std::string& package_name);
bool install_package(const std::string& package_name);
bool remove_package(const std::string& package_name);
std::vector<Package> search_packages(const std::string& query);
void display_package_info(const Package& pkg);
bool load_installed_packages();
void save_installed_packages();
bool extract_package(const std::string& package_path, const std::string& extract_path);
bool create_package_directories();
std::string get_package_cache_dir();
std::string get_package_install_dir();

// Command handler function declarations
void handle_install(const std::vector<std::string>& package_names);
void handle_install_local(const std::string& package_file);
void handle_remove(const std::vector<std::string>& package_names);
void handle_search(const std::string& query);

int main(int argc, char** argv) {
    CLI::App app{"The package manager for the Foxglove Linux distribution."};
    app.set_version_flag("-v,--version", "0.1.0");

    // Install command
    auto install_cmd = app.add_subcommand("install", "Install one or more packages.");
    std::vector<std::string> install_packages;
    install_cmd->add_option("packages", install_packages, "Names of the package(s) to install")->required();

    // Install local command
    auto install_local_cmd = app.add_subcommand("install-local", "Install a package from a local .fox file.");
    std::string local_package_file;
    install_local_cmd->add_option("package-file", local_package_file, "Path to local .fox package file")->required();

    // Remove command
    auto remove_cmd = app.add_subcommand("remove", "Remove one or more packages.");
    std::vector<std::string> remove_packages;
    remove_cmd->add_option("packages", remove_packages, "Names of the package(s) to remove")->required();

    // Search command
    auto search_cmd = app.add_subcommand("search", "Search for a package in repositories.");
    std::string search_query;
    search_cmd->add_option("query", search_query, "Search query")->required();

    // Set required to ensure a command is given
    app.require_subcommand(1);

    // Parse arguments
    CLI11_PARSE(app, argc, argv);

    // Execute the correct command
    if (app.get_subcommand(install_cmd)) {
        handle_install(install_packages);
    } else if (app.get_subcommand(install_local_cmd)) {
        handle_install_local(local_package_file);
    } else if (app.get_subcommand(remove_cmd)) {
        handle_remove(remove_packages);
    } else if (app.get_subcommand(search_cmd)) {
        handle_search(search_query);
    }

    return 0;
}

// Helper function implementations
void initialize_package_database() {
    // Simulated package database
    package_database = {
        {"firefox", {"firefox", "1.0.0", "Web browser", {"gtk"}, "Mozilla", "MPL-2.0", false}},
        {"vim", {"vim", "8.2", "Text editor", {}, "Bram Moolenaar", "Vim", false}},
        {"git", {"git", "2.34.1", "Version control system", {}, "Git Contributors", "GPL-2.0", false}},
        {"gtk", {"gtk", "3.24", "GUI toolkit", {}, "GNOME", "LGPL-2.1", false}},
        {"nano", {"nano", "6.0", "Simple text editor", {}, "GNU", "GPL-3.0", false}},
        {"curl", {"curl", "7.80.0", "Data transfer tool", {}, "Daniel Stenberg", "curl", false}},
    };
}

bool check_dependencies(const std::vector<std::string>& dependencies) {
    for (const auto& dep : dependencies) {
        if (installed_packages.find(dep) == installed_packages.end()) {
            std::cout << "Missing dependency: " << dep << std::endl;
            return false;
        }
    }
    return true;
}

std::string get_package_cache_dir() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }
    return std::string(home) + "/.fox/cache";
}

std::string get_package_install_dir() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }
    return std::string(home) + "/.fox/packages";
}

bool create_package_directories() {
    std::filesystem::create_directories(get_package_cache_dir());
    std::filesystem::create_directories(get_package_install_dir());
    return true;
}

bool download_package(const std::string& package_name) {
    std::cout << "Downloading " << package_name << "... ";
    std::cout.flush();
    
    std::string cache_dir = get_package_cache_dir();
    std::string package_file = cache_dir + "/" + package_name + ".fox";
    
    // In a real implementation, this would download from a repository
    // For now, we'll create a dummy package file for demonstration
    std::ofstream dummy_package(package_file, std::ios::binary);
    if (!dummy_package) {
        std::cout << "failed (cannot create package file)" << std::endl;
        return false;
    }
    
    // Create a simple tar.xz-like structure (simplified)
    dummy_package << "FOX_PACKAGE_" << package_name << "_" << package_database[package_name].version;
    dummy_package.close();
    
    std::cout << "done." << std::endl;
    return true;
}

bool extract_package(const std::string& package_path, const std::string& extract_path) {
    // In a real implementation, this would extract tar.xz
    // For now, we'll simulate extraction by creating directories
    std::filesystem::create_directories(extract_path);
    
    // Create some dummy files to simulate package contents
    std::ofstream((extract_path + "/bin/" + std::filesystem::path(package_path).stem().string()).c_str());
    std::ofstream((extract_path + "/lib/lib" + std::filesystem::path(package_path).stem().string() + ".so").c_str());
    
    return true;
}

bool install_package(const std::string& package_name) {
    std::cout << "Installing " << package_name << "... ";
    std::cout.flush();
    
    std::string cache_dir = get_package_cache_dir();
    std::string install_dir = get_package_install_dir();
    std::string package_file = cache_dir + "/" + package_name + ".fox";
    std::string package_install_path = install_dir + "/" + package_name;
    
    // Extract package
    if (!extract_package(package_file, package_install_path)) {
        std::cout << "failed (extraction error)" << std::endl;
        return false;
    }
    
    // Set proper permissions
    std::filesystem::permissions(package_install_path, 
                                std::filesystem::perms::owner_all | std::filesystem::perms::group_read | std::filesystem::perms::others_read,
                                std::filesystem::perm_options::replace);
    
    installed_packages.insert(package_name);
    package_database[package_name].installed = true;
    save_installed_packages();
    
    std::cout << "done." << std::endl;
    return true;
}

bool remove_package(const std::string& package_name) {
    std::cout << "Removing " << package_name << "... ";
    std::cout.flush();
    
    std::string install_dir = get_package_install_dir();
    std::string package_path = install_dir + "/" + package_name;
    
    // Remove package files
    if (std::filesystem::exists(package_path)) {
        std::filesystem::remove_all(package_path);
    }
    
    installed_packages.erase(package_name);
    package_database[package_name].installed = false;
    save_installed_packages();
    
    std::cout << "done." << std::endl;
    return true;
}

bool load_installed_packages() {
    std::string cache_dir = get_package_cache_dir();
    std::string installed_file = cache_dir + "/installed.txt";
    
    std::ifstream file(installed_file);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                installed_packages.insert(line);
                if (package_database.find(line) != package_database.end()) {
                    package_database[line].installed = true;
                }
            }
        }
        file.close();
    }
    return true;
}

void save_installed_packages() {
    std::string cache_dir = get_package_cache_dir();
    std::filesystem::create_directories(cache_dir);
    std::string installed_file = cache_dir + "/installed.txt";
    
    std::ofstream file(installed_file);
    if (file.is_open()) {
        for (const auto& pkg : installed_packages) {
            file << pkg << std::endl;
        }
        file.close();
    }
}

std::vector<Package> search_packages(const std::string& query) {
    std::vector<Package> results;
    for (const auto& [name, pkg] : package_database) {
        if (name.find(query) != std::string::npos ||
            pkg.description.find(query) != std::string::npos) {
            results.push_back(pkg);
        }
    }
    return results;
}

void display_package_info(const Package& pkg) {
    std::cout << pkg.name << " (" << pkg.version << ") - " << pkg.description;
    if (pkg.installed) std::cout << " [installed]";
    std::cout << std::endl;
}

std::string get_package_root_dir() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }
    return std::string(home) + "/.fox/root";
}

std::string get_temp_extract_dir() {
    return "/tmp/fox-extract";
}

bool run_command(const std::string& cmd) {
    int ret = system(cmd.c_str());
    return (ret == 0);
}

bool real_download_package(const std::string& package_name, const std::string& url_base) {
    std::string cache_dir = get_package_cache_dir();
    std::string package_file = cache_dir + "/" + package_name + ".fox";
    std::string url = url_base + "/" + package_name + ".fox";
    std::cout << "Downloading " << url << "... ";
    std::cout.flush();
    std::string cmd = "curl -fsSL -o '" + package_file + "' '" + url + "'";
    if (!run_command(cmd)) {
        std::cout << "failed." << std::endl;
        return false;
    }
    std::cout << "done." << std::endl;
    return true;
}

bool real_extract_package(const std::string& package_file, const std::string& extract_dir) {
    std::filesystem::remove_all(extract_dir);
    std::filesystem::create_directories(extract_dir);
    std::string cmd = "tar -xJf '" + package_file + "' -C '" + extract_dir + "'";
    return run_command(cmd);
}

bool parse_fox_json(const std::string& extract_dir, json& fox_meta) {
    std::ifstream fox_json_file(extract_dir + "/fox.json");
    if (!fox_json_file.is_open()) return false;
    fox_json_file >> fox_meta;
    return true;
}

bool real_install_package(const std::string& package_name, const std::string& url_base) {
    std::string cache_dir = get_package_cache_dir();
    std::string package_file = cache_dir + "/" + package_name + ".fox";
    std::string extract_dir = get_temp_extract_dir();
    std::string root_dir = get_package_root_dir();

    if (!real_extract_package(package_file, extract_dir)) {
        std::cout << "Extraction failed." << std::endl;
        return false;
    }
    // Parse fox.json
    json fox_meta;
    if (!parse_fox_json(extract_dir, fox_meta)) {
        std::cout << "Missing or invalid fox.json." << std::endl;
        return false;
    }
    // Copy files to root_dir (simulate system root)
    std::string copy_cmd = "cp -a '" + extract_dir + "/.' '" + root_dir + "'";
    if (!run_command(copy_cmd)) {
        std::cout << "Failed to copy files." << std::endl;
        return false;
    }
    // Track installed files
    std::ofstream manifest(cache_dir + "/" + package_name + ".manifest");
    for (auto& p : std::filesystem::recursive_directory_iterator(extract_dir)) {
        if (p.is_regular_file()) {
            manifest << std::filesystem::relative(p.path(), extract_dir) << std::endl;
        }
    }
    manifest.close();
    installed_packages.insert(package_name);
    package_database[package_name].installed = true;
    save_installed_packages();
    std::cout << "Installed " << package_name << " successfully." << std::endl;
    return true;
}

// --- Command Implementations ---

void handle_install(const std::vector<std::string>& package_names) {
    initialize_package_database();
    load_installed_packages();
    create_package_directories();
    std::string url_base = "https://example.com/foxpkgs"; // TODO: set real repo URL
    for(const auto& pkg : package_names) {
        if (package_database.find(pkg) == package_database.end()) {
            std::cout << "Package not found: " << pkg << std::endl;
            continue;
        }
        if (package_database[pkg].installed) {
            std::cout << pkg << " is already installed." << std::endl;
            continue;
        }
        if (!check_dependencies(package_database[pkg].dependencies)) {
            std::cout << "Cannot install " << pkg << " due to missing dependencies." << std::endl;
            continue;
        }
        if (!real_download_package(pkg, url_base)) {
            std::cout << "Failed to download " << pkg << std::endl;
            continue;
        }
        if (!real_install_package(pkg, url_base)) {
            std::cout << "Failed to install " << pkg << std::endl;
            continue;
        }
    }
}

void handle_install_local(const std::string& package_file) {
    if (!std::filesystem::exists(package_file)) {
        std::cout << "Package file not found: " << package_file << std::endl;
        return;
    }

    std::cout << "Installing local package from " << package_file << "..." << std::endl;

    std::string extract_dir = get_temp_extract_dir();
    std::string root_dir = get_package_root_dir();
    
    // Extract the package
    if (!real_extract_package(package_file, extract_dir)) {
        std::cout << "Failed to extract package." << std::endl;
        return;
    }

    // Parse fox.json to get package name
    json fox_meta;
    if (!parse_fox_json(extract_dir, fox_meta)) {
        std::cout << "Missing or invalid fox.json in package." << std::endl;
        return;
    }

    std::string package_name = fox_meta["name"];
    std::cout << "Package name: " << package_name << std::endl;

    // Copy files to root directory
    std::string copy_cmd = "cp -a '" + extract_dir + "/.' '" + root_dir + "'";
    if (!run_command(copy_cmd)) {
        std::cout << "Failed to copy files to installation directory." << std::endl;
        return;
    }

    // Track installed files
    std::string cache_dir = get_package_cache_dir();
    std::ofstream manifest(cache_dir + "/" + package_name + ".manifest");
    for (auto& p : std::filesystem::recursive_directory_iterator(extract_dir)) {
        if (p.is_regular_file()) {
            manifest << std::filesystem::relative(p.path(), extract_dir) << std::endl;
        }
    }
    manifest.close();

    // Update package database
    load_installed_packages();
    installed_packages.insert(package_name);
    if (package_database.find(package_name) == package_database.end()) {
        package_database[package_name] = {
            package_name,
            fox_meta.value("version", "unknown"),
            fox_meta.value("description", ""),
            fox_meta.value("dependencies", std::vector<std::string>{}),
            fox_meta.value("maintainer", ""),
            fox_meta.value("license", ""),
            true
        };
    } else {
        package_database[package_name].installed = true;
    }
    save_installed_packages();

    std::cout << "Successfully installed " << package_name << "!" << std::endl;
}

void handle_remove(const std::vector<std::string>& package_names) {
    initialize_package_database();
    load_installed_packages();
    
    for(const auto& pkg : package_names) {
        if (package_database.find(pkg) == package_database.end()) {
            std::cout << "Package not found: " << pkg << std::endl;
            continue;
        }
        if (!package_database[pkg].installed) {
            std::cout << pkg << " is not installed." << std::endl;
            continue;
        }
        // Check if any other package depends on this one
        bool is_dependency = false;
        for (const auto& [name, p] : package_database) {
            if (p.installed && std::find(p.dependencies.begin(), p.dependencies.end(), pkg) != p.dependencies.end()) {
                std::cout << pkg << " is required by installed package: " << name << std::endl;
                is_dependency = true;
                break;
            }
        }
        if (is_dependency) continue;
        if (!remove_package(pkg)) {
            std::cout << "Failed to remove " << pkg << std::endl;
        }
    }
}

void handle_search(const std::string& query) {
    initialize_package_database();
    load_installed_packages();
    auto results = search_packages(query);
    if (results.empty()) {
        std::cout << "No packages found matching '" << query << "'." << std::endl;
    } else {
        std::cout << "Search results for '" << query << "':" << std::endl;
        for (const auto& pkg : results) {
            display_package_info(pkg);
        }
    }
}