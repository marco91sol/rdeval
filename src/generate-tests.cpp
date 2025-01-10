#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <cstdio>

#include "validate.h"

int main(int, char **argv) {
    std::cout << "WARNING: only run this program if gfastats is in a working state" << std::endl;
    std::cout << "WARNING: previous validate files will be deleted" << std::endl;
    std::cout << "continue? (Y/N) ";
    std::string input;
    std::cin >> input;
    if(input != "Y" && input != "y") {
        std::cout << "validate generation cancelled" << std::endl;
        std::exit(0);
    }
    std::cout << "deleting old validate files..." << std::endl;

    for(auto &file : list_dir("validateFiles")) {
        if(getFileExt(file) != "tst") continue; // dont delete README
        file = "validateFiles/"+file;
        if(remove(file.c_str()) != 0) {
            std::cerr << "error deleting <" << file << ">" << std::endl;
            return -1;
        }
    }

    std::cout << "generating new validate files..." << std::endl;

    std::string exePath = getExePath(argv[0]);

    const std::map<std::set<std::string>, std::vector<std::string>> ext_args = {
        {{"fasta", "fasta.gz", "fastq", "fastq.gz", "bam", "cram"}, {"", "5", "-f \"l>10\"", "-f \"l<10\"", "-f \"l=10\"", "--sequence-report"}},
        {{"fasta", "fasta.gz", "fastq", "fastq.gz", "bam", "cram"}, {"-o /tmp/test.fastq", "-o /tmp/test.fastq.gz", "-o /tmp/test.bam", "-o /tmp/test.cram"}},
        {{"rd"}, {"", "--md5"}}
        //  {{set of test file extensions}, {list of command line args to run with}}
    };

    const std::map<std::set<std::string>, std::vector<std::string>> file_args = {
        {{"random1.fastq"}, {"-s u", "-s s", "-s h", "-s c"}},
        {{"random2.fastq"}, {"-f 'l>10000' -qa", "-f 'l<9000 & q>10' -qa", "-f 'l>9000 & q>90' -qa", "-f 'q>89' -ql", "-i testFiles/random2.ls", "-e testFiles/random2.ls", "--sample 0.3 --random-seed 42"}},
        {{"random2.rd"}, {"-f 'l>10000' -qa", "-f 'l<9000 & q>10' -qa", "-f 'l>9000 & q>90' -qa", "-f 'q>89' -ql"}},
        {{"random3.bam"}, {""}}
    //     {{"random1.fasta", "random1.fasta.gz", "random1.fastq", "random1.fastq.gz", "random1.gfa"}, {"-r testFiles/random1.fastq.gz", "-a testFiles/random1.agp --stats", "-a testFiles/random1.agp --stats -ofa"}},
    //     {{"random1.fasta"}, {"-k testFiles/random1.instructions.sak", "-ofa -k testFiles/random1.instructions.sak", "-ofa -k testFiles/random1.hc.sak", "-ofa -k testFiles/random1.hdc.sak"}},
    //     {{"random2.noseq.gfa"}, {""}},
    //     {{"random1.gfa2"}, {"-k testFiles/random1.gfa2.instructions.sak"}},
    //     {{"random1.fasta"}, {"Header2", "-r testFiles/random1.fastq"}},
    //     {{"random4.fasta"}, {""}}
    
        
    //  {{set of test file paths}, {list of command line args to run with}}
    };

    const std::set<std::string> exclude {"agp", "sak"};

    for(const std::string &file : list_dir("testFiles")) {
        std::string ext = getFileExt(file);
        if(exclude.count(ext)) continue;
        for(auto pair : ext_args) {
            if(!pair.first.count(ext)) continue;
            for(auto args : pair.second) {
                genTest(exePath, file, args);
            }
        }
    }

    std::fstream fstream;
    for(const auto &pair : file_args) {
        for(const std::string &file : pair.first) {
            fstream.open("testFiles/"+file);
            if(!fstream) continue;
            fstream.close();
            for(const std::string &args : pair.second) {
                genTest(exePath, file, args);
            }
        }
    }

    std::exit(EXIT_SUCCESS);
}
