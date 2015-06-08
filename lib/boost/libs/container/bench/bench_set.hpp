//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2013-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_BENCH_BENCH_SET_HPP
#define BOOST_CONTAINER_BENCH_BENCH_SET_HPP

#include <iostream>
#include <boost/timer/timer.hpp>
#include <algorithm> //sort
#include <exception>
#include <sstream>
#include <iomanip>
#include <boost/container/vector.hpp>
#include <boost/container/string.hpp>

using boost::timer::cpu_timer;
using boost::timer::cpu_times;
using boost::timer::nanosecond_type;

#ifdef NDEBUG
static const std::size_t NElements = 1000;
#else
static const std::size_t NElements = 100;
#endif

#ifdef NDEBUG
static const std::size_t NIter = 500;
#else
static const std::size_t NIter = 50;
#endif

void compare_times(cpu_times time_numerator, cpu_times time_denominator){
   std::cout << ((double)time_numerator.wall/(double)time_denominator.wall) << std::endl;
   std::cout << "----------------------------------------------" << '\n' << std::endl;
}

boost::container::vector<int> sorted_unique_range_int;
boost::container::vector<int> sorted_range_int;
boost::container::vector<int> random_unique_range_int;
boost::container::vector<int> random_range_int;

void fill_range_ints()
{
   //sorted_unique_range_int
   sorted_unique_range_int.resize(NElements);
   for(std::size_t i = 0, max = sorted_unique_range_int.size(); i != max; ++i){
      sorted_unique_range_int[i] = static_cast<int>(i);
   }
   //sorted_range_int
   sorted_range_int = sorted_unique_range_int;
   sorted_range_int.insert(sorted_range_int.end(), sorted_unique_range_int.begin(), sorted_unique_range_int.end());
   std::sort(sorted_range_int.begin(), sorted_range_int.end());

   //random_range_int
   std::srand(0);
   random_range_int.assign(sorted_range_int.begin(), sorted_range_int.end());
   std::random_shuffle(random_range_int.begin(), random_range_int.end());
   //random_unique_range_int
   std::srand(0);
   random_unique_range_int.assign(sorted_unique_range_int.begin(), sorted_unique_range_int.end());
   std::random_shuffle(random_unique_range_int.begin(), random_unique_range_int.end());
}

boost::container::vector<boost::container::string> sorted_unique_range_string;
boost::container::vector<boost::container::string> sorted_range_string;
boost::container::vector<boost::container::string> random_unique_range_string;
boost::container::vector<boost::container::string> random_range_string;

void fill_range_strings()
{
   boost::container::string model_s;
   model_s.append(sizeof(boost::container::string), '*');

   //sorted_unique_range_int
   sorted_unique_range_string.resize(NElements);
   std::stringstream sstr;

   for(std::size_t i = 0, max = sorted_unique_range_string.size(); i != max; ++i){
      sstr.str(std::string());
      sstr << std::setfill('0') << std::setw(10) << i;
      sorted_unique_range_string[i] = model_s;
      const std::string &s = sstr.str();
      sorted_unique_range_string[i].append(s.begin(), s.end());
   }
   //sorted_range_string
   sorted_range_string = sorted_unique_range_string;
   sorted_range_string.insert(sorted_range_string.end(), sorted_unique_range_string.begin(), sorted_unique_range_string.end());
   std::sort(sorted_range_string.begin(), sorted_range_string.end());

   //random_range_string
   std::srand(0);
   random_range_string.assign(sorted_range_string.begin(), sorted_range_string.end());
   std::random_shuffle(random_range_string.begin(), random_range_string.end());
   //random_unique_range_string
   std::srand(0);
   random_unique_range_string.assign(sorted_unique_range_string.begin(), sorted_unique_range_string.end());
   std::random_shuffle(random_unique_range_string.begin(), random_unique_range_string.end());
}

template<class T>
struct range_provider;

template<>
struct range_provider<int>
{
   typedef boost::container::vector<int> type;

   static type &sorted_unique()
   {  return sorted_unique_range_int;  }

   static type &sorted()
   {  return sorted_range_int;  }

   static type &random_unique()
   {  return random_unique_range_int;  }

   static type &random()
   {  return random_range_int;  }
};

template<>
struct range_provider<boost::container::string>
{
   typedef boost::container::vector<boost::container::string> type;

   static type &sorted_unique()
   {  return sorted_unique_range_string;  }

   static type &sorted()
   {  return sorted_range_string;  }

   static type &random_unique()
   {  return random_unique_range_string;  }

   static type &random()
   {  return random_range_string;  }
};

template<typename C>
cpu_times copy_destroy_time(boost::container::vector<typename C::value_type> &unique_range)
{
   cpu_timer copy_timer, assign_timer, destroy_timer;

   cpu_timer total_time;

   for(std::size_t i = 0; i != NIter; ++i){
      {
         C c(unique_range.begin(), unique_range.end());
         total_time.resume();
         copy_timer.resume();
         C caux(c);
         copy_timer.stop();
         assign_timer.resume();
         c = caux;
         assign_timer.stop();
         destroy_timer.resume();
      }
      destroy_timer.stop();
      total_time.stop();
   }
   total_time.stop();

   std::cout << " Copy sorted range             " << boost::timer::format(copy_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Assign sorted range           " << boost::timer::format(assign_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Destroy                       " << boost::timer::format(destroy_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Total time =                  " << boost::timer::format(total_time.elapsed(), boost::timer::default_places, "%ws\n") << std::endl;
   return total_time.elapsed();
}

template<typename C>
cpu_times construct_time( boost::container::vector<typename C::value_type> &unique_range
                        , boost::container::vector<typename C::value_type> &range
                        , const char *RangeType)
{
   cpu_timer sur_timer, sr_timer;

   cpu_timer total_time;

   for(std::size_t i = 0; i != NIter; ++i){
      {
         sur_timer.resume();
         total_time.resume();
         C c(unique_range.begin(), unique_range.end());
         sur_timer.stop();
         total_time.stop();
      }
      {
         total_time.resume();
         sr_timer.resume();
         C c(range.begin(), range.end());
         sr_timer.stop();
         total_time.stop();
      }
   }

   std::cout << " Construct " << RangeType << " unique_range " << boost::timer::format(sur_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Construct " << RangeType << " range        " << boost::timer::format(sr_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Total time =                 " << boost::timer::format(total_time.elapsed(), boost::timer::default_places, "%ws\n") << std::endl;
   return total_time.elapsed();
}

template<typename C>
cpu_times insert_time( boost::container::vector<typename C::value_type> &unique_range
                     , boost::container::vector<typename C::value_type> &range
                     , const char *RangeType)
{
   cpu_timer ur_timer,r_timer;
   ur_timer.stop();r_timer.stop();

   cpu_timer total_time;
   total_time.resume();

   for(std::size_t i = 0; i != NIter; ++i){
      {
         total_time.resume();
         ur_timer.resume();
         C c;
         c.insert(unique_range.begin(), unique_range.end());
         ur_timer.stop();
         total_time.stop();
      }
      {
         total_time.resume();
         r_timer.resume();
         C c;
         c.insert(range.begin(), range.end());
         r_timer.stop();
         total_time.stop();
      }
   }

   std::cout << " Insert " << RangeType << " unique_range " << boost::timer::format(ur_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Insert " << RangeType << " range        " << boost::timer::format(r_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Total time =              " << boost::timer::format(total_time.elapsed(), boost::timer::default_places, "%ws\n") << std::endl;
   return total_time.elapsed();
}

template<typename Iterator>
bool check_not_end(boost::container::vector<Iterator> &iterators, Iterator itend, std::size_t number_of_ends = 0)
{
   std::size_t end_count = 0;
   for(std::size_t i = 0, max = iterators.size(); i != max; ++i){
      if(iterators[i] == itend && (++end_count > number_of_ends) )
         return false;
   }
   return true;
}

template<typename Iterator>
bool check_all_not_empty(boost::container::vector< std::pair<Iterator, Iterator > > &iterator_pairs)
{
   for(std::size_t i = 0, max = iterator_pairs.size(); i != max; ++i){
      if(iterator_pairs[i].first == iterator_pairs[i].second)
         return false;
   }
   return true;
}

template<typename C>
cpu_times search_time(boost::container::vector<typename C::value_type> &unique_range, const char *RangeType)
{
   cpu_timer find_timer, lower_timer, upper_timer, equal_range_timer, count_timer;

   C c(unique_range.begin(), unique_range.end());

   cpu_timer total_time;
   total_time.resume();

   boost::container::vector<typename C::iterator> v_it(NElements);
   boost::container::vector< std::pair<typename C::iterator, typename C::iterator> > v_itp(NElements);

   for(std::size_t i = 0; i != NIter; ++i){
      //Find
      {
         find_timer.resume();
         for(std::size_t rep = 0; rep != 2; ++rep)
         for(std::size_t i = 0, max = unique_range.size(); i != max; ++i){
            v_it[i] = c.find(unique_range[i]);
         }
         find_timer.stop();
         if(!check_not_end(v_it, c.end())){
            std::cout << "ERROR! find all elements not found" << std::endl;
         }
      }
      //Lower
      {
         lower_timer.resume();
         for(std::size_t rep = 0; rep != 2; ++rep)
         for(std::size_t i = 0, max = unique_range.size(); i != max; ++i){
            v_it[i] = c.lower_bound(unique_range[i]);
         }
         lower_timer.stop();
         if(!check_not_end(v_it, c.end())){
            std::cout << "ERROR! lower_bound all elements not found" << std::endl;
         }
      }
      //Upper
      {
         upper_timer.resume();
         for(std::size_t rep = 0; rep != 2; ++rep)
         for(std::size_t i = 0, max = unique_range.size(); i != max; ++i){
            v_it[i] = c.upper_bound(unique_range[i]);
         }
         upper_timer.stop();
         if(!check_not_end(v_it, c.end(), 1u)){
            std::cout << "ERROR! upper_bound all elements not found" << std::endl;
         }
      }
      //Equal
      {
         equal_range_timer.resume();
         for(std::size_t rep = 0; rep != 2; ++rep)
         for(std::size_t i = 0, max = unique_range.size(); i != max; ++i){
            v_itp[i] = c.equal_range(unique_range[i]);
         }
         equal_range_timer.stop();
         if(!check_all_not_empty(v_itp)){
            std::cout << "ERROR! equal_range all elements not found" << std::endl;
         }
      }
      //Count
      {
         std::size_t count = 0;
         count_timer.resume();
         for(std::size_t rep = 0; rep != 2; ++rep)
         for(std::size_t i = 0, max = unique_range.size(); i != max; ++i){
            count += c.count(unique_range[i]);
         }
         count_timer.stop();
         if(count/2 != c.size()){
            std::cout << "ERROR! count all elements not found" << std::endl;
         }
      }
   }
   total_time.stop();

   std::cout << " Find        " << RangeType << " " << boost::timer::format(find_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Lower Bound " << RangeType << " " << boost::timer::format(lower_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Upper Bound " << RangeType << " " << boost::timer::format(upper_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Equal Range " << RangeType << " " << boost::timer::format(equal_range_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Count       " << RangeType << " " << boost::timer::format(count_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Total time =      " << boost::timer::format(total_time.elapsed(), boost::timer::default_places, "%ws\n") << std::endl;
   return total_time.elapsed();
}

template<typename C>
void extensions_time(boost::container::vector<typename C::value_type> &sorted_unique_range)
{
   cpu_timer sur_timer,sur_opt_timer;
   sur_timer.stop();sur_opt_timer.stop();

   for(std::size_t i = 0; i != NIter; ++i){
      {
         sur_timer.resume();
         C c(sorted_unique_range.begin(), sorted_unique_range.end());
         sur_timer.stop();
      }
      {
         sur_opt_timer.resume();
         C c(boost::container::ordered_unique_range, sorted_unique_range.begin(), sorted_unique_range.end());
         sur_opt_timer.stop();
      }

   }
   std::cout << " Construct sorted_unique_range             " << boost::timer::format(sur_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << " Construct sorted_unique_range (extension) " << boost::timer::format(sur_opt_timer.elapsed(), boost::timer::default_places, "%ws\n");
   std::cout << "Extension/Standard: ";
   compare_times(sur_opt_timer.elapsed(), sur_timer.elapsed());
}

template<class BoostClass, class StdClass>
void launch_tests(const char *BoostContName, const char *StdContName)
{
   typedef range_provider<typename BoostClass::value_type> get_range_t;
   try {
      std::cout << "**********************************************" << '\n';
      std::cout << "**********************************************" << '\n';
      std::cout << '\n';
      std::cout << BoostContName << " .VS " <<  StdContName         << '\n';
      std::cout << '\n';
      std::cout << "**********************************************" << '\n';
      std::cout << "**********************************************" << '\n' << std::endl;
      {
         std::cout << "Copy/Assign/Destroy benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = copy_destroy_time< BoostClass >(get_range_t::sorted_unique());

         std::cout << "Copy/Assign/Destroy benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = copy_destroy_time< StdClass >(get_range_t::sorted_unique());

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Ordered construct benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = construct_time< BoostClass >(get_range_t::sorted_unique(), get_range_t::sorted(), "(ord)");

         std::cout << "Ordered construct benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = construct_time< StdClass >(get_range_t::sorted_unique(), get_range_t::sorted(), "(ord)");;

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Random construct benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = construct_time< BoostClass >(get_range_t::random_unique(), get_range_t::random(), "(rnd)");

         std::cout << "Random construct benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = construct_time< StdClass >(get_range_t::random_unique(), get_range_t::random(), "(rnd)");;

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Ordered Insert benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = insert_time< BoostClass >(get_range_t::sorted_unique(), get_range_t::sorted(), "(ord)");

         std::cout << "Ordered Insert benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = insert_time< StdClass >(get_range_t::sorted_unique(), get_range_t::sorted(), "(ord)");

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Random Insert benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = insert_time< BoostClass >(get_range_t::random_unique(), get_range_t::random(), "(rnd)");

         std::cout << "Random Insert benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = insert_time< StdClass >(get_range_t::random_unique(), get_range_t::random(), "(rnd)");

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Ordered Search benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = search_time< BoostClass >(get_range_t::sorted_unique(), "(ord)");

         std::cout << "Ordered Search benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = search_time< StdClass >(get_range_t::sorted_unique(), "(ord)");

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Random Search benchmark:" << BoostContName << std::endl;
         cpu_times boost_set_time = search_time< BoostClass >(get_range_t::random_unique(), "(rnd)");

         std::cout << "Random Search benchmark:" << StdContName << std::endl;
         cpu_times std_set_time = search_time< StdClass >(get_range_t::random_unique(), "(rnd)");

         std::cout << BoostContName << "/" << StdContName << ": ";
         compare_times(boost_set_time, std_set_time);
      }
      {
         std::cout << "Extensions benchmark:" << BoostContName << std::endl;
         extensions_time< BoostClass >(get_range_t::sorted_unique());
      }

   }catch(std::exception e){
      std::cout << e.what();
   }
}

#endif   //#ifndef BOOST_CONTAINER_BENCH_BENCH_SET_HPP
