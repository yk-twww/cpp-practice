#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include "util.h"



class BigramDecoder
{
  static const std::string Unknown;
  static const std::string Bos;
  static const std::string Eos;
  std::map<std::pair<std::string, std::string>, double> bi_probs_;
  std::map<std::string, double>                         uni_probs_;
public:
  void read_model(const std::string& f_name);
  double eval_sentence(const std::vector<std::string>& sentence);
  void do_test(const std::string& f_name);
};

const std::string BigramDecoder::Unknown = "##<UNKNOWN>##";
const std::string BigramDecoder::Bos     = "##<BOS>##";
const std::string BigramDecoder::Eos     = "##<EOS>##";


void BigramDecoder::read_model(const std::string& f_name)
{
  std::ifstream ifs(f_name.data());
  if (ifs.fail()) {
    std::cerr << "File can't open." << std::endl;
    exit(1);
  }

  std::string              line;
  std::vector<std::string> columns;
  std::stringstream        ss;
  double                   prob;
  while (true) {
    getline(ifs, line);
    if (line == "") { break; }
    boost::split(columns, line, boost::is_space());
    ss << columns.at(1);
    ss >> prob;
    uni_probs_[columns.at(0)] = prob;
    ss.clear();
  }

  std::pair<std::string, std::string> wd_pair;
  while (getline(ifs, line)) {
    boost::split(columns, line, boost::is_space());
    ss << columns.at(2);
    ss >> prob;
    wd_pair.first  = columns.at(0);
    wd_pair.second = columns.at(1);
    bi_probs_[wd_pair] = prob;
    ss.clear();
  }
}

double BigramDecoder::eval_sentence(const std::vector<std::string>& sentence)
{
  std::pair<std::string, std::string> wd_pair;
  int sent_n = sentence.size();
  double prob = 0.0;

  wd_pair.first = Bos;
  for (int i = 0; i < sent_n; ++i) {
    wd_pair.second = sentence[i];
    prob += (bi_probs_.find(wd_pair) == bi_probs_.end())?
      ((uni_probs_.find(wd_pair.second) == uni_probs_.end())? uni_probs_[Unknown] : uni_probs_[wd_pair.second]) : log_sum_exp(bi_probs_[wd_pair], uni_probs_[wd_pair.second]);
    wd_pair.first = wd_pair.second;
  }
  wd_pair.second = Eos;
    prob += (bi_probs_.find(wd_pair) == bi_probs_.end())?
      ((uni_probs_.find(wd_pair.second) == uni_probs_.end())? uni_probs_[Unknown] : uni_probs_[wd_pair.second]) : log_sum_exp(bi_probs_[wd_pair], uni_probs_[wd_pair.second]);

    return prob;
}
      


void BigramDecoder::do_test(const std::string& f_name)
{
  std::ifstream ifs(f_name.data());
  if (ifs.fail()) {
    std::cerr << "File can't open." << std::endl;
    exit(1);
  }

  std::string line;
  std::vector<std::string> sentence;
  int i, sent_len, word_n;
  double prob, whole_prob;

  whole_prob = 0.0;
  word_n     = 0;
  while (getline(ifs, line)) {
    boost::split(sentence, line, boost::is_space());
    prob = eval_sentence(sentence);
    std::cout << prob << ":";
    for (i = 0, sent_len = sentence.size(); i < sent_len; ++i) {
      std::cout << " " << sentence.at(i);
    }
    std::cout << std::endl;
    whole_prob += prob;
    word_n += sent_len;
  }

  double perp;
  perp = exp(-(whole_prob / word_n));
  std::cout << perp << std::endl;
}



int main(int argc, char** argv)
{
  BigramDecoder b_dec;

  std::string model_f(argv[1]);
  std::string test_f(argv[2]);

  b_dec.read_model(model_f);
  b_dec.do_test(test_f);

  return 0;
}

  
