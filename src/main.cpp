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
//Added this include
#include "CLI/CLI.hpp"

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
void simulate_download(const std::string& package_name);
void simulate_install(const std::string& package_name);
void simulate_remove(const std::string& package_name);
std::vector<Package> search_packages(const std::string& query);
void display_package_info(const Package& pkg);

// Command handler function declarations
void handle_install(const std::vector<std::string>& package_names);
void handle_remove(const std::vector<std::string>& package_names);
void handle_search(const std::string& query);

int main(int argc, char** argv) {
    CLI::App app{"The package manager for the Foxglove Linux distribution."};
    app.set_version_flag("-v,--version", "0.1.0");

    // Install command
    auto install_cmd = app.add_subcommand("install", "Install one or more packages.");
    std::vector<std::string> install_packages;
    install_cmd->add_option("packages", install_packages, "Names of the package(s) to install")->required();

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

void simulate_download(const std::string& package_name) {
    std::cout << "Downloading " << package_name << "... ";
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "done." << std::endl;
}

void simulate_install(const std::string& package_name) {
    std::cout << "Installing " << package_name << "... ";
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "done." << std::endl;
    installed_packages.insert(package_name);
    package_database[package_name].installed = true;
}

void simulate_remove(const std::string& package_name) {
    std::cout << "Removing " << package_name << "... ";
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "done." << std::endl;
    installed_packages.erase(package_name);
    package_database[package_name].installed = false;
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

// --- Command Implementations ---

void handle_install(const std::vector<std::string>& package_names) {
    initialize_package_database();
    for(const auto& pkg : package_names) {
        if (package_database.find(pkg) == package_database.end()) {
            std::cout << "Package not found: " << pkg << std::endl;
            continue;
        }
        if (package_database[pkg].installed) {
            std::cout << pkg << " is already installed." << std::endl;
            continue;
        }
        // Check dependencies
        if (!check_dependencies(package_database[pkg].dependencies)) {
            std::cout << "Cannot install " << pkg << " due to missing dependencies." << std::endl;
            continue;
        }
        simulate_download(pkg);
        simulate_install(pkg);
    }
}

void handle_remove(const std::vector<std::string>& package_names) {
    initialize_package_database();
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
        simulate_remove(pkg);
    }
}

void handle_search(const std::string& query) {
    initialize_package_database();
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