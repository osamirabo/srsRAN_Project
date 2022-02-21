/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_INTERVAL_H
#define SRSGNB_INTERVAL_H

#include "srsran/srslog/bundled/fmt/format.h"
#include "srsran/support/srsran_assert.h"
#include <cassert>
#include <string>
#include <type_traits>

namespace srsgnb {

/// Representation of an interval between two numeric-types with the math representation [start, stop)
template <typename T>
class interval
{
  static_assert(std::is_trivially_copyable<T>::value, "Expected to be trivially copyable");

public:
  interval() : start_(T{}), stop_(T{}) {}
  interval(T start_point, T stop_point) : start_(start_point), stop_(stop_point) { assert(start_ <= stop_); }

  T start() const { return start_; }
  T stop() const { return stop_; }

  bool empty() const { return stop_ == start_; }

  /// Interval length
  auto length() const -> decltype(std::declval<T>() - std::declval<T>()) { return stop_ - start_; }

  void set(T start_point, T stop_point)
  {
    srsran_assert(stop_point >= start_point, "interval::set called for invalid range points");
    start_ = start_point;
    stop_  = stop_point;
  }

  /// Increase the interval length, maintaining the same starting point
  void resize_by(T len)
  {
    // Detect length overflows
    srsran_assert(std::is_unsigned<T>::value or (len >= 0 or length() >= -len), "Resulting interval would be invalid");
    stop_ += len;
  }

  /// Set the interval length, maintaining the same starting point
  void resize_to(T len)
  {
    srsran_assert(std::is_unsigned<T>::value or len >= 0, "Interval width must be positive");
    stop_ = start_ + len;
  }

  /// Move interval by an offset
  void displace_by(int offset)
  {
    start_ += offset;
    stop_ += offset;
  }

  /// Move interval start to provided starting point
  void displace_to(T start_point)
  {
    stop_  = start_point + length();
    start_ = start_point;
  }

  /// If this and other intervals overlap
  bool overlaps(interval other) const { return start_ < other.stop_ and other.start_ < stop_; }

  /// If interval contains provided point
  bool contains(T point) const { return start_ <= point and point < stop_; }

  /// Assign to this interval the intersection between this and the provided interval
  interval<T>& intersect(const interval<T>& other)
  {
    if (not overlaps(other)) {
      *this = interval<T>{};
    } else {
      start_ = std::max(start(), other.start());
      stop_  = std::min(stop(), other.stop());
    }
    return *this;
  }

private:
  T start_;
  T stop_;
};

template <typename T>
bool operator==(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start() == rhs.start() and lhs.stop() == rhs.stop();
}

template <typename T>
bool operator!=(const interval<T>& lhs, const interval<T>& rhs)
{
  return not(lhs == rhs);
}

template <typename T>
bool operator<(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start() < rhs.start() or (lhs.start() == rhs.start() and lhs.stop() < rhs.stop());
}

/// Make union of intervals. If intervals do not overlap, the empty interval is returned
template <typename T>
interval<T> operator|(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return {std::min(lhs.start(), rhs.start()), std::max(lhs.stop(), rhs.stop())};
}

template <typename T>
interval<T> make_union(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs | rhs;
}

/// Make intersection of intervals
template <typename T>
interval<T> operator&(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return interval<T>{std::max(lhs.start(), rhs.start()), std::min(lhs.stop(), rhs.stop())};
}

template <typename T>
interval<T> make_intersection(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs & rhs;
}

} // namespace srsgnb

namespace fmt {

/// Format intervals with the notation [start, stop)
template <typename T>
struct formatter<srsgnb::interval<T> > : public formatter<T> {
  template <typename FormatContext>
  auto format(const srsgnb::interval<T>& interv, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "[{}, {})", interv.start(), interv.stop());
  }
};

} // namespace fmt

#endif // SRSGNB_INTERVAL_H
