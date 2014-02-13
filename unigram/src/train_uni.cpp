#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>


void count_word(const std::string* f_name, std::map<std::string, int>* counter)
{
  std::ifstream ifs(f_name->data());
  std::string sentence;
  std::string word;
  std::vector<std::string> words;
  int i, sent_n;

  if (ifs.fail()) {
    std::cerr << "File can't open." << std::endl;
    exit(1);
  }

  while (getline(ifs, sentence)) {
    boost::split(words, sentence, boost::is_space());
    sent_n = words.size();
    for (i = 0; i < sent_n; ++i) {
      word = words.at(i);
      (*counter)[word] = (counter->find(word) == counter->end())? 1 : (*counter)[word] + 1;
    }
  }
}

void dump_model(std::map<std::string, int>* counter, double lambda)
{
  int    type_n;
  double mean_p;
  double denom_inv;
  double prob;

  int token_n = 0;
  std::map<std::string, int>::iterator it = counter->begin();
  for (; it != counter->end(); ++it) {
    token_n += (*it).second;
  }

  type_n    = counter->size();
  denom_inv = 1.0 / token_n;
  mean_p    = 1.0 / (type_n + 1);
  it = counter->begin();
  for (; it != counter->end(); ++it) {
    prob = (*it).second * denom_inv * lambda + mean_p * (1.0 - lambda);
    std::cout << (*it).first << " " << prob << std::endl;
  }
  std::cout << "##UNKNOWN##" << " " << mean_p * (1.0 - lambda) << std::endl;
}


int main(int argc, char** argv)
{
  double lambda;
  lambda = (argc == 3)? strtod(argv[2], NULL) : 0.95;
  std::string f_name(argv[1]);
  std::map<std::string, int> counter;

  count_word(&f_name, &counter);
  dump_model(&counter, lambda);

  return 0;
}
