#ifndef __RWSTL_ALGORITHM_H__
#define __RWSTL_ALGORITHM_H__
/***************************************************************************
 *
 * STL-(almost)-compliant algorithms
 * 
 * $Id: algo.h,v 7.1 1996/01/11 23:21:07 jims Exp $
 *
 * $$RW_INSERT_HEADER "tlyrs.str"
 *
 ****************************************************************************
 * 
 * $Log: algo.h,v $
 * Revision 7.1  1996/01/11 23:21:07  jims
 * Move to revision 7.1
 *
 * Revision 1.2  1996/01/05 00:41:55  griswolf
 * Scopus #2160: Fix typo, add the remaining inplace set algorithms.
 *
 */

template <class IIter1, class IIter2, class OIter, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after an insert at any iterator
set_union_inplace(
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		OIter self_inserter,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      ++first1;
    else if (comp(*first2,*first1))
      self_inserter = *first2++;
    else {
      ++first1;
      ++first2;
    }
  }
  while (first2 != bound2)
    self_inserter = *first2++;
}

template <class Container, class IIter1, class IIter2, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after a deletion at any iterator
set_intersection_inplace(
		Container& con,
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      con.erase(first1++);
    else if (comp(*first2,*first1))
      ++first2;
    else {
      ++first1;
      ++first2;
    }
  }
  con.erase(first1,bound1);
}

template <class Container, class IIter1, class IIter2, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after a deletion at any iterator
set_difference_inplace(
		Container& con,
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      ++first1;
    else if (comp(*first2,*first1))
      ++first2;
    else {
      con.erase(first1++);
      ++first2;
    }
  }
}

template <class Container, class IIter1, class IIter2, class Compare>
void
// depends on the "inplace" container
//   -- being sorted
//   -- remaining sorted after a deletion or insertion at any iterator
set_symmetric_difference_inplace(
		Container& con,
		IIter1 first1, IIter1 bound1,
		IIter2 first2, IIter2 bound2,
		Compare comp
	       )
{
  while (first1 != bound1 && first2 != bound2) {
    if (comp(*first1,*first2))
      first1++;
    else if (comp(*first2,*first1))
      con.insert(*first2++);
    else {
      con.erase(first1++);
      ++first2;
    }
  }
  while (first2 != bound2) con.insert(*first2++); 
}

#endif /*__RWSTL_ALGORITHM_H__*/
