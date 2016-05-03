#ifndef SFU_CONTAINER_TRAITS_H_
#define SFU_CONTAINER_TRAITS_H_

// #include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sfu {

// container_traits::<C<T>>value_type;
// container_traits::<C<T>>add_element(C& c, const T& t);
template<class C>
struct container_traits {}; 

// std::map, std::multimap and unordered_*

template<class C>
struct dictionary_container_traits;

template<
  template<class,class,class,class> class C,
  class K, class T,
  template<class> class O,
  class A>
struct dictionary_container_traits<C<K,T,O<K>,A>> {
  using value_type = std::pair<K,T>;
  static void add_element(C<K,T,O<K>,A>& c, const std::pair<K,T>& t) {
    c.insert(t);
  }
};

template<class... Args>
struct container_traits<std::map<Args...>> :
    public dictionary_container_traits<std::map<Args...>>
{};

template<class... Args>
struct container_traits<std::multimap<Args...>> :
    public dictionary_container_traits<std::multimap<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_map<Args...>> :
    public dictionary_container_traits<std::unordered_map<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_multimap<Args...>> :
    public dictionary_container_traits<std::unordered_multimap<Args...>>
{};

// std::set, std::multiset and unordered_*

template<class C>
struct associative_container_traits;

template<
  template<class,class,class> class C,
  class T,
  template<class> class O,
  class A>
struct associative_container_traits<C<T,O<T>,A>> {
  using value_type = T;
  static void add_element(C<T,O<T>,A>& c, const T& t) {
    c.insert(t);
  }
};

template<class... Args>
struct container_traits<std::set<Args...>> :
    public associative_container_traits<std::set<Args...>>
{};

template<class... Args>
struct container_traits<std::multiset<Args...>> :
    public associative_container_traits<std::multiset<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_set<Args...>> :
    public associative_container_traits<std::unordered_set<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_multiset<Args...>> :
    public associative_container_traits<std::unordered_multiset<Args...>>
{};

// list containers:
// std::vector, std::deque, std::list

template<class C>
struct list_container_traits;

template<
  template<class,class> class C,
  class T,
  class A>
struct list_container_traits<C<T, A>> {
  using value_type = T;
  static void add_element(C<T,A>& c, const T& t) {
    c.push_back(t);
  }
};

template<class... Args>
struct container_traits<std::vector<Args...>> :
    public list_container_traits<std::vector<Args...>>
{};

template<class... Args>
struct container_traits<std::deque<Args...>> :
    public list_container_traits<std::deque<Args...>>
{};

template<class... Args>
struct container_traits<std::list<Args...>> :
    public list_container_traits<std::list<Args...>>
{};

// std::forward_list

template<class C>
struct forward_list_container_traits;

template<
  template<class,class> class C,
  class T,
  class A>
struct forward_list_container_traits<C<T, A>> {
  using value_type = T;
  static void add_element(C<T,A>& c, const T& t) {
    c.push_front(t);
  }
};

template<class... Args>
struct container_traits<std::forward_list<Args...>> :
    public forward_list_container_traits<std::forward_list<Args...>>
{};

// Ordered lists
// std::queue, std::stack, std::priority_queue

template<class C>
struct ordered_list_container_traits;

template<
  template<class,class> class C,
  class T,
  class A>
struct ordered_list_container_traits<C<T, A>> {
  using value_type = T;
  static void add_element(C<T,A>& c, const T& t) {
    c.push(t);
  }
};

template<class... Args>
struct container_traits<std::stack<Args...>> :
    public ordered_list_container_traits<std::stack<Args...>>
{};

template<class... Args>
struct container_traits<std::queue<Args...>> :
    public ordered_list_container_traits<std::queue<Args...>>
{};

template<class... Args>
struct container_traits<std::priority_queue<Args...>> :
    public ordered_list_container_traits<std::priority_queue<Args...>>
{};

}  // namespace sfu

#endif
