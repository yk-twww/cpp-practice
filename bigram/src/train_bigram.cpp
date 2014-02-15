#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <deque>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include "util.h"



class BigramLM
{
public:
  static const std::string Unknown;
  static const std::string Bos;
  static const std::string Eos;

  std::map<std::pair<std::string, std::string>, int> bi_count_;
  std::map<std::string, int>                         uni_count_;
  int bos_num_;
  int token_num_;

  BigramLM(void) { bos_num_ = token_num_ = 0;}
  void count_token(const std::string& f_name);
  void calc_probs(double lmd1, double lmd2,
		  std::map<std::pair<std::string, std::string>, double>& bi_probs,
		  std::map<std::string, double>& uni_probs);
  void dump_model(const std::string& f_name,
		  const std::map<std::pair<std::string, std::string>, double>& bi_probs,
		  const std::map<std::string, double>& uni_probs);
};

const std::string BigramLM::Unknown = "##<UNKNOWN>##";
const std::string BigramLM::Bos     = "##<BOS>##";
const std::string BigramLM::Eos     = "##<EOS>##";

void BigramLM::count_token(const std::string& f_name)
{
  std::ifstream ifs(f_name.data());
  if (ifs.fail()) {
    std::cerr << "File can't open." << std::endl;
    exit(1);
  }

  std::string raw_line;
  std::string word;
  std::deque<std::string> words;
  std::deque<std::string>::iterator it;
  std::pair<std::string, std::string> wd_pair;

  while (getline(ifs, raw_line)) {
    boost::split(words, raw_line, boost::is_space());
    token_num_ += words.size() + 1;
    words.push_back(Eos);
    wd_pair.first = Bos;
    ++bos_num_;

    for (it = words.begin(); it != words.end(); ++it) {
      word = (*it);
      wd_pair.second = word;
      uni_count_[word] = (uni_count_.find(word) == uni_count_.end())?
	1 : uni_count_[word] + 1;
      bi_count_[wd_pair] = (bi_count_.find(wd_pair) == bi_count_.end())?
	1 : bi_count_[wd_pair] + 1;
      wd_pair.first = wd_pair.second;
    }
  }
}

void BigramLM::calc_probs(double lmd1, double lmd2,
	       std::map<std::pair<std::string, std::string>, double>& bi_probs,
	       std::map<std::string, double>& uni_probs)
{
  std::pair<std::string, std::string> wd_pair;
  std::map<std::string, int>::iterator pre_it, next_it;
  int pre_occ, bi_occ;
  double log_lmd2 = log(lmd2);

  for (pre_it = uni_count_.begin(); pre_it != uni_count_.end(); ++pre_it) {
    wd_pair.first = (*pre_it).first;
    pre_occ       = (*pre_it).second;
    for (next_it = uni_count_.begin(); next_it != uni_count_.end(); ++next_it) {
      wd_pair.second = (*next_it).first;
      if (bi_count_.find(wd_pair) == bi_count_.end()) { continue; }
      bi_occ = bi_count_[wd_pair];
      bi_probs[wd_pair] = log_lmd2 + log(bi_occ) - log(pre_occ);
    }
  }

  wd_pair.first = Bos;
  pre_occ       = bos_num_;
  for (next_it = uni_count_.begin(); next_it != uni_count_.end(); ++next_it) {
    wd_pair.second = (*next_it).first;
    if (bi_count_.find(wd_pair) == bi_count_.end()) { continue; }
    bi_occ = bi_count_[wd_pair];
    bi_probs[wd_pair] = log_lmd2 + log(bi_occ) - log(pre_occ);
  }


  int voc_num = uni_count_.size() + 1;

  double denom      = log(token_num_);
  double mean_prob  = log(1.0 - lmd1) - log(voc_num);
  double log_lmd1   = log(lmd1);
  double log_lmd2_c = log(1.0 - lmd2);

  double uni_prob, add_prob;
  for (next_it = uni_count_.begin(); next_it != uni_count_.end(); ++next_it) {
    uni_prob  = log_lmd1 + log((*next_it).second) - denom;
    uni_probs[(*next_it).first] = log_lmd2_c + log_sum_exp(uni_prob, mean_prob);
  }
  uni_probs[Unknown] = log_lmd2_c + log(1.0 - lmd1) + mean_prob;
}

void BigramLM::dump_model(const std::string& f_name,
			  const std::map<std::pair<std::string, std::string>, double>& bi_probs,
			  const std::map<std::string, double>& uni_probs)
{
  std::map<std::string, double>::const_iterator it;
  for (it = uni_probs.begin(); it != uni_probs.end(); ++it) {
    std::cout << (*it).first << " " << (*it).second << std::endl;
  }
  std::cout << std::endl;

  std::map<std::pair<std::string, std::string>, double>::const_iterator it2;
  for (it2 = bi_probs.begin(); it2 != bi_probs.end(); ++it2) {
    std::cout << (*it2).first.first << " " << (*it2).first.second;
    std::cout << " " << (*it2).second << std::endl;
  }
}



int main(int argc, char** argv)
{
  BigramLM bi_lm;
  std::string f_name(argv[1]);

  double lmd1, lmd2;
  if (argc == 4) {
    lmd1 = strtod(argv[2], NULL);
    lmd2 = strtod(argv[3], NULL);
  } else {
    lmd1 = lmd2 = 0.95;
  }

  bi_lm.count_token(f_name);
  std::cerr << "counting end." << std::endl;

  std::map<std::pair<std::string, std::string>, double> bi_probs;
  std::map<std::string, double>                         uni_probs;
  bi_lm.calc_probs(lmd1, lmd2, bi_probs, uni_probs);
  std::cerr << "calculating end." << std::endl;

  bi_lm.dump_model(f_name, bi_probs, uni_probs);
  std::cerr << "dumping end." << std::endl;

  return 0;
}

