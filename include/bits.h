/*
 * Copyright 2014 Nicola Gigante
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BITVECTOR_BITS_H
#define BITVECTOR_BITS_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <memory>
#include <cassert>
#include <cmath>
#include <iterator>

#include <iostream>
#include <iomanip>

namespace bitvector
{
    namespace details
    {
        template<size_t W>
        struct word_t { };
        
        template<>
        struct word_t<8> {
            using type = uint8_t;
        };
        
        template<>
        struct word_t<16> {
            using type = uint16_t;
        };
        
        template<>
        struct word_t<32> {
            using type = uint32_t;
        };
        
        template<>
        struct word_t<64> {
            using type = uint64_t;
        };
        
        // FIXME: test __uint128_t availability
        template<>
        struct word_t<128> {
            using type = __uint128_t;
        };
    }
    
    /*
     * Generic configurable-size word type. The size parameter is in bits.
     */
    template<size_t W>
    using word_t = typename details::word_t<W>::type;
    
    // Extracts from the word the bits in the range [begin, end),
    // counting from zero from the LSB.
    template<typename T>
    T get_bitfield(T word, size_t begin, size_t end)
    {
        const size_t W = sizeof(T) * 8;
        
        assert(end >= begin);
        assert(begin < W);
        assert(end < W);
        
        if(begin == end)
            return 0;
        
        const T mask = std::numeric_limits<T>::max() >> (W - end + begin);
        
        return (word >> begin) & mask;
    }
    
    template<typename T, typename U>
    void set_bitfield(T *word, size_t begin, size_t end, U value)
    {
        const size_t W = sizeof(T) * 8;
        
        assert(end >= begin);
        assert(begin < W);
        assert(end < W);
        
        const T mask = std::numeric_limits<T>::max() >> (W - end + begin);
        
        *word |= (value & mask) << begin;
    }
    
    struct bytes;
    struct bits;
    
    namespace details
    {
        struct size_tag;
        struct index_tag;
        
        /*
         * Class that wraps an integer to represent a size or an index.
         *
         * The Unit template parameter can be one of the two 'bytes' or 'bits'
         * structs declared above, and designates the unit of measure.
         *
         * The Kind parameter says if the type represents a size or an index.
         * Conversion between bits and bytes is different depending on the kind.
         *
         * wrappers of different units and/or different kinds do not convert
         * from each other.
         *
         * The user should not use this class directly, but rather the size<>
         * and index<> template types declared below
         */
        template<typename Unit, typename Kind, typename T = size_t>
        class wrap_t
        {
            T _v;
        public:
            constexpr wrap_t() = default;
            constexpr wrap_t(T v) : _v(v) { }
            
            constexpr wrap_t(wrap_t const&) = default;
            constexpr wrap_t(wrap_t &&) = default;
            
            wrap_t &operator=(wrap_t const&) = default;
            wrap_t &operator=(wrap_t &&) = default;
            
            constexpr operator T() const { return _v; }
        };
    
    }
    
    template<typename Unit, typename T = size_t>
    using size = details::wrap_t<Unit, details::size_tag, T>;
    
    template<typename Unit, typename T = size_t>
    using index = details::wrap_t<Unit, details::index_tag, T>;
    
    /*
     * Conversion functions between bits and bytes
     */
    template<typename Kind, typename T>
    constexpr
    details::wrap_t<bits, Kind, T> to_bits(details::wrap_t<bytes, Kind, T> B)
    {
        return T(B) * 8;
    }
    
    template<typename T>
    constexpr
    details::wrap_t<bytes, details::size_tag, T>
    to_bytes(details::wrap_t<bits, details::size_tag, T> b)
    {
        return T(ceil(float(T(b)) / 8));
    }
    
    template<typename T>
    constexpr
    details::wrap_t<bytes, details::index_tag, T>
    to_bytes(details::wrap_t<bits, details::index_tag, T> b)
    {
        return T(b) / 8;
    }
    
} // namespace bitvector
#endif
