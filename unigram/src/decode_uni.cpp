#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <fstream>
#include <sstream>



void read_model(const std::string* f_name, std::map<std::string, double>* probs)
{
  std::ifstream ifs(f_name->data());
  if (ifs.fail()) {
    std::cerr << "File can't open." << std::endl;
    exit(1);
  }

  std::string              line;
  std::vector<std::string> columns;
  std::stringstream        ss;
  double                   prob;
  while (getline(ifs, line)) {
    boost::split(columns, line, boost::is_space());
    ss << columns.at(1);
    ss >> prob;
    (*probs)[columns.at(0)] = log(prob);
    ss.clear();
  }
}


double eval_sentence(const std::vector<std::string>& sentence, std::map<std::string, double>& probs)
{
  int         sent_n   = sentence.size();
  double      log_prob = 0.0;
  std::string token;

  for (int i = 0; i < sent_n; ++i) {
    token = (probs.find(sentence.at(i)) == probs.end())? "##UNKNOWN##" : sentence.at(i);
    log_prob += probs[token];
  }
  return log_prob;
}

void do_test(const std::string& f_name, std::map<std::string, double>& probs)
{
  std::ifstream ifs(f_name.data());
  if (ifs.fail()) {
    std::cerr << "File can't open." << std::endl;
    exit(1);
  }

  std::string line;
  std::vector<std::string> sentence;
  int i, sent_len, word_n;
  double whole_log_prob, log_prob;
  double perp;

  whole_log_prob = 0.0;
  word_n = 0;
  while (getline(ifs, line)) {
    boost::split(sentence, line, boost::is_space());

    log_prob = eval_sentence(sentence, probs);
    sent_len = sentence.size();
    perp = exp( -(log_prob / sent_len));
    whole_log_prob += log_prob;
    word_n += sent_len;
    std::cout << perp << ":";
    for (i = 0; i < sent_len; ++i) {
      std::cout << " " << sentence.at(i);
    }
    std::cout << std::endl;
  }

  double whole_perp = exp( -(whole_log_prob / word_n));
  std::cout << whole_perp << std::endl;
}


int main(int argc, char** argv)
{
  std::string model_f(argv[1]);
  std::map<std::string, double> probs;
  read_model(&model_f, &probs);

  std::string test_f(argv[2]);
  do_test(test_f, probs);

  return 0;
}
