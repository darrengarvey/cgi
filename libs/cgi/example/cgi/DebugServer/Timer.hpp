
#undef max
#undef min
/**
 * You need Boost.Chrono from the Boost sandbox svn.
 *
 * See http://svn.boost.org/trac/boost/browser/sandbox/chrono/.
 */
#include <boost/chrono/chrono.hpp>

//using namespace boost::chrono;

template< class Clock >
class Timer
{
  typename Clock::time_point start;
public:

  Timer() : start( Clock::now() ) {}

  typename Clock::duration elapsed() const
  {
    return Clock::now() - start;
  }

  double seconds() const
  {
    return elapsed().count() * ((double)Clock::period::num/Clock::period::den);
  }

  double latency() const
  {
      return (-(Clock::now() - Clock::now())).count();
  }
};
