// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Collections;

namespace IceUtil
{

    public sealed class Arrays
    {
        public static bool Equals(object[] arr1, object[] arr2)
        {
            if(object.ReferenceEquals(arr1, arr2))
            {
                return true;
            }

            if((arr1 == null && arr2 != null) || (arr1 != null && arr2 == null))
            {
                return false;
            }

            if(arr1.Length == arr2.Length)
            {
                for(int i = 0; i < arr1.Length; i++)
                {
                    if(arr1[i] == null)
                    {
                        if(arr2[i] != null)
                        {
                            return false;
                        }
                    }
                    else if(!arr1[i].Equals(arr2[i]))
                    {
                        return false;
                    }
                }

                return true;
            }

            return false;           
        }

        public static bool Equals(Array arr1, Array arr2)
        {
            if(object.ReferenceEquals(arr1, arr2))
            {
                return true;
            }

            if((arr1 == null && arr2 != null) || (arr1 != null && arr2 == null))
            {
                return false;
            }

            if(arr1.Length == arr2.Length)
            {
                IEnumerator e1 = arr1.GetEnumerator();
                IEnumerator e2 = arr2.GetEnumerator();
                while(e1.MoveNext())
                {
                    e2.MoveNext();
                    if(e1.Current == null)
                    {
                        if(e2.Current != null)
                        {
                            return false;
                        }
                    }
                    else if(!e1.Current.Equals(e2.Current))
                    {
                        return false;
                    }
                }

                return true;
            }

            return false;           
        }

        public static int GetHashCode(object[] arr)
        {
            int h = 0;

            for(int i = 0; i < arr.Length; i++)
            {
                h = 5 * h + arr[i].GetHashCode();
            }

            return h;           
        }

        public static int GetHashCode(Array arr)
        {
            int h = 0;

            foreach(object o in arr)
            {
                h = 5 * h + o.GetHashCode();
            }

            return h;           
        }

        public static void Sort(ref ArrayList array, IComparer comparator)
        {
            //
            // This Sort method implements the merge sort algorithm
            // which is a stable sort (unlike the Sort method of the
            // System.Collections.ArrayList which is unstable).
            //
            Sort1(ref array, 0, array.Count, comparator);
        }

        private static void Sort1(ref ArrayList array, int begin, int end, IComparer comparator)
        {
            int mid;
            if(end - begin <= 1)
            {
                return;
            }

            mid = (begin + end) / 2;
            Sort1(ref array, begin, mid, comparator);
            Sort1(ref array, mid, end, comparator);
            Merge(ref array, begin, mid, end, comparator);
        }

        private static void Merge(ref ArrayList array, int begin, int mid, int end, IComparer comparator)
        {
            int i = begin;
            int j = mid;
            int k = 0;

            object[] tmp = new object[end - begin];
            while(i < mid && j < end)
            {
                if(comparator.Compare(array[i], array[j]) <= 0)
                {
                    tmp[k++] = array[i++];
                }
                else
                {
                    tmp[k++] = array[j++];
                }
            }

            while(i < mid)
            {
                tmp[k++] = array[i++];
            }
            while(j < end) 
            {
                tmp[k++] = array[j++];
            }
            for(i = 0; i < (end - begin); ++i) 
            {
                array[begin + i] = tmp[i];
            }
        }
    }

}
