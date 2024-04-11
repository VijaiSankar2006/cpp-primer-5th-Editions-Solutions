/************************************************************************************************************************************************************
 * @file explorer.cpp
 * @brief code to learn to use std::filesystem
 * @date 2024-03-22
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************************************************************************************************/

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#include <map>
#include <iomanip>
#include <cstring>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;
 
class Directory {
    friend std::ostream & operator<<(std::ostream &os, Directory &dir);
    public : 
        explicit Directory(const fs::path &curr_, size_t indent_ = 0) : curr(curr_), indent(indent_){
            for(auto iter = fs::directory_iterator(curr); iter != fs::directory_iterator(); ++iter) {
                std::string fname = iter->path().filename();
                if (fs::is_directory(*iter)) {
                        dir_list[iter->path()] = new Directory(iter->path(), indent + 1);
                } else if (fs::is_regular_file(*iter)) { 
                        file_list.insert(iter->path());
                }
            }
        }

        void set_exclude_extension(const std::initializer_list<std::string> &il) {
            for (auto s : il) {
                exclude_extensionlist.insert(s);
            }
        }

        const std::set<std::string> & get_exclude_extensionlist() {
            return exclude_extensionlist;
        }

        void set_exclude_dir(const std::initializer_list<std::string> &il) {
            for (auto s : il) {
                exclude_dirlist.insert(s);
            }
        }

        void set_exclude_file(const std::initializer_list<std::string> &il) {
            for (auto s : il) {
                exclude_filelist.insert(s);
            } 
        }

        const std::set<std::string> & get_exclude_dir() {
            return exclude_dirlist;
        }

        const std::set<std::string> & get_exclude_file() {
            return exclude_filelist;
        }

        void set_exclude_dot_dir(bool val) {
            exclude_dot_dir = val;
        }

        void set_exclude_dot_files(bool val) {
            exclude_dot_files = val;
        }        
 
        ~Directory() {
            for(auto p : dir_list) {
                delete p.second;
            }
        }

        static void merge_files(std::ostream &os, const std::vector<fs::path> &merge_file_list);
        

    private :
        fs::path curr;
        size_t indent;
        std::set<fs::path> file_list;
        std::map<fs::path, Directory *> dir_list;
        static std::string hyperlink(const fs::path &p);
        static std::set<std::string> exclude_dirlist;
        static std::set<std::string> exclude_filelist;
        static std::set<std::string> exclude_extensionlist;
        static bool exclude_dot_dir;
        static bool exclude_dot_files;
        static bool check_exclude_dot_dir(const std::string &fname);
        static bool check_exclude_dot_files(const std::string &fname);
        static bool is_root;
        static std::string add_space(const fs::path &p);
};

std::set<std::string> Directory::exclude_dirlist;
std::set<std::string> Directory::exclude_filelist;
std::set<std::string> Directory::exclude_extensionlist;
bool Directory::exclude_dot_dir = false;
bool Directory::exclude_dot_files = false;
bool Directory::is_root = true;

void Directory::merge_files(std::ostream &os, const std::vector<fs::path>& merge_file_list){
    os << std::endl;
    for (auto& p : merge_file_list) {
        std::ifstream ifile(p);
        if (!ifile) {
            std::cout << "unable to open file" << p.filename() << std::endl;
        }
        std::string line; 
        while (getline(ifile, line)) {
            os << line << std::endl;
        }
    }
}

std::string Directory::add_space(const fs::path &p) {
    size_t min_len = 20;
    size_t len = strlen(p.filename().c_str());
    if (len < min_len) {
        return std::string(min_len - len, ' ');
    }
    return std::string();
} 

std::string Directory::hyperlink(const fs::path &p) {
    return std::string("<a href = \"./codes/") + std::string(fs::relative(p)) + "\">" + std::string(p.filename()) + "</a>";
}

bool Directory::check_exclude_dot_dir(const std::string &fname) {
    if (exclude_dot_dir) {
        if (fname[0] == '.') {
            return true;
        } else {
            return false;
        }
    } 
    return false;
}

bool Directory::check_exclude_dot_files(const std::string &fname) {
    if (exclude_dot_files) {
        if (fname[0] == '.') {
            return true;
        } else {
            return false;
        }
    } 
    return false;
}

std::ostream & operator<<(std::ostream &os, Directory &dir) {
    std::string indent_str("&nbsp;");
    for (size_t i = 0; i < dir.indent * 3; ++i) {
        indent_str += indent_str;
    }
    os << "<ul>";
    for (auto p : dir.file_list) {
        if (dir.exclude_filelist.find(p.filename()) == dir.exclude_filelist.end() 
            && dir.exclude_extensionlist.find(p.extension()) == dir.exclude_extensionlist.end()
            && !Directory::check_exclude_dot_files(p.filename().c_str())) {     
            os << "<li>" << dir.hyperlink(p) << "</li>";  
        }    
    } 
     
    for (auto p : dir.dir_list) {   
        if (dir.exclude_dirlist.find(p.first.filename()) == dir.exclude_dirlist.end() && !Directory::check_exclude_dot_dir(p.first.filename().c_str())) {
            os << "<li><details><summary>" << p.first.stem().string() << "</summary>"<< std::endl;
            os << *p.second;
            os << "</details></li>";
        }        
    }

    return os;
}    

int main() {
    std::ofstream ofile("/home/vijai/github/cpp-primer-5th-Editions-Solutions/navigation.txt");
    fs::path curr("/home/vijai/github/cpp-primer-5th-Editions-Solutions/_codes");
    std::cout << "displaying contents of : " << curr << std::endl;
    Directory d1(curr);
    d1.set_exclude_extension({".exe",".out"});
    d1.set_exclude_file({"rooster"});
    d1.set_exclude_dot_dir(true);
    d1.set_exclude_dot_files(true);
    d1.set_exclude_dir({"docs"});
    ofile << "<aside><h1>Index</h1>";
    ofile << d1 << std::endl;
    ofile << "</aside>"; 
    return 0;
}
