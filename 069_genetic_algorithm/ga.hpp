#ifndef GA751_HPP
#define GA751_HPP
#include <unistd.h>

#include <cstdlib>
#include <utility>
#include <vector>

#include "per_thread_random.hpp"
#include "quicksort.hpp"   // XXX 0: Must be symlinked
#include "threadpool.hpp"  // XXX 0: Must be symlinked

/*
 * GeneticAlgorithm is templated. Fitness.cpp contains
 * EBSGene which has all of the below defined for you 
 * to implement.
 */
template<typename Gene,
         typename InitParams,
         Gene makeRandomGene(const InitParams & init),
         Gene breed(const Gene & g1, const Gene & g2),
         double fitness(const Gene & g)>
class GeneticAlgorithm {
  size_t total_popsize;
  size_t survive_popsize;
  size_t max_iter;
  std::vector<std::pair<Gene, double> > population1;
  std::vector<std::pair<Gene, double> > population2;
  std::vector<std::pair<Gene, double> > * prev_gen;
  std::vector<std::pair<Gene, double> > * next_gen;

 private:
  /*
   * This function should populate population1 with std::pairs 
   * containing a random gene and the fitness of that gene.
   * You should split work evenly across threads. 
   */
  void initialize_population(ECE751::ThreadPool<> & tp, const InitParams & ip) {
    auto task = [ip, this](int lower, int upper) {
      for (int j = lower; j < upper; j++) {
        *prev_gen[j] = makeRandomGene(ip);
      }
    };
    for (int i = 0; i < tp.size(); i++) {
      if (i == tp.size() - 1) {
        tp.runTaskToCompletion(task(i * (prev_gen->size() / tp.size()),
                                    (i + 1) * (prev_gen->size() / tp.size())));
      }
      else {
        tp.enqueue(task(i * (prev_gen->size() / tp.size()),
                        (i + 1) * (prev_gen->size() / tp.size())));
      }
    }
  }
  /*
   * Sorts the given population. You should define a comparison function and 
   * pass it into parallel_qsort
   */
  void sort_pop(ECE751::ThreadPool<> & tp,
                std::vector<std::pair<Gene, double> > * population) {
    auto c = [](std::pair<Gene, double> a, std::pair<Gene, double> b) {
      return a.second < b.second;
    };
    parallel_qsort(tp, population, c);
  }

  /*
   * Populates next_gen by breeding genes from the previous generation
   * together. You should use per_thread_random to determine your random
   * indexes.
   * You should split work evenly across threads.
   */
  void breed_pop(ECE751::ThreadPool<> & tp) {
    auto task = [this](int lower, int upper) {
      for (int j = lower; j < upper; j++) {
        int t1 = per_thread_random() % prev_gen->size();
        int t2 = per_thread_random() % prev_gen->size();
        *next_gen[j] = breed(*prev_gen[t1], *prev_gen[t2]);
      }
    };
    for (int i = 0; i < tp.size(); i++) {
      if (i == tp.size() - 1) {
        tp.runTaskToCompletion(task(i * (next_gen->size() / tp.size()),
                                    (i + 1) * (next_gen->size() / tp.size())));
      }
      else {
        tp.enqueue(task(i * (next_gen->size() / tp.size()),
                        (i + 1) * (next_gen->size() / tp.size())));
      }
    }
  }

 public:
  GeneticAlgorithm(size_t total, size_t keep, size_t iter) :
      total_popsize(total),
      survive_popsize(keep),
      max_iter(iter),
      population1(total),
      population2(total),
      prev_gen(&population1),
      next_gen(&population2) {}

  /*
   * Runs the genetic algorithm. There are 3 steps:
   *  1. Initialize population.
   *  2. Perform max_iter runs of the genetic algorithm. This involves
   *     sorting the previous generation, breeding, then swapping the
   *     prev and next generation.
   *  3. Sort the final result and return the resulting gene. 
   */
  Gene run(ECE751::ThreadPool<> & tp, const InitParams & ip) {
    initialize_population(tp, ip);
    for (size_t i = 0; i < max_iter; i++) {
      sort_pop(tp, prev_gen);
      breed_pop(tp);
      std::swap(prev_gen, next_gen);
    }
    std::sort(next_gen->begin(), next->end());
    return next_gen[next_gen->size() - 1];
    //return Gene(); }
  };

#endif
