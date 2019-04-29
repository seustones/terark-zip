
#include "rank_select_fewzero.hpp"

namespace terark {

template <size_t P, size_t W>
size_t rank_select_few<P, W>::lower_bound(size_t val) const {
  const uint8_t *base = m_mempool.data();
  const uint8_t *p = base + ((m_layer == 1) ? 0 : m_offset[m_layer - 2]);
  for (int i = m_layer - 2; i >= 0; --i) {
      for (const uint8_t *e = base + m_offset[i + 1]; p < e; p += W) {
          if (val_at_ptr(p) > val)
              break;
      }
      if(p > base + m_offset[i]) p -= W;
      p = base + (i ? m_offset[i - 1] : 0) + (p - base - m_offset[i]) * 256;
  }
  for(const uint8_t *e = base + m_offset[0]; p < e; p += W){
      if (val_at_ptr(p) >= val)
          break;
  }
  return (p - base) / W;
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::lower_bound(size_t val, size_t &hint) const {
   /*
   if (hint < (P ? m_num1 : m_num0))
    if (val_a_logi(hint) == val)
      return hint;
      */
  hint = lower_bound(val);
  return hint;
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::select_complement(size_t id) const {
  assert(id < (P ? m_num0 : m_num1));
  const uint8_t *base = m_mempool.data();
  if(id < val_at_ptr(base)){
      return id;
  }
  if(id + (P ? m_num1 : m_num0) > val_at_ptr(base + m_offset[0] - W)){
      return id + (P ? m_num1 : m_num0);
  }
  const uint8_t *p = base + ((m_layer == 1) ? 0 : m_offset[m_layer - 2]);
  const uint8_t *e;
  size_t pos = 0;
  for( int i = m_layer - 2; i >= 0; --i){
    for(e = base + m_offset[i + 1]; p < e ;){
        if(id + pos < val_at_ptr(p)) break;
        p += W;
        pos += 1ULL << (8 * i + 8);
    }
    if(pos){
        p -= W;
        pos -= 1ULL << (8 * i + 8);
    }
    p = base + (i ? m_offset[i - 1] : 0) + (p - (base + m_offset[i])) * 256;
  }
  for(e = base + m_offset[0]; p < e ;){
      if(id + pos < val_at_ptr(p)) break;
      p += W;
      pos ++;
  }
  return id + pos;
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::select_complement(size_t id, size_t &hint) const {
      /*
  size_t n = P ? m_num1 : m_num0;
  if (hint == 0) {
    if (id < val_a_logi(0))
      return id;
    else if (id < val_a_logi(1))
      return id - 1;
  } else if (hint == n - 1) {
    if (id + n > val_a_logi(n - 1))
      return id + n - 1;
    else if (id + n - 1 > val_a_logi(n - 2))
      return id + n - 2;
  } else if (hint < n) {
    size_t pre = val_a_logi(hint - 1);
    size_t now = val_a_logi(hint);
    if ((pre <= id + hint) && (id + hint < now))
      return id + hint - 1;
    size_t nxt = val_a_logi(hint + 1);
    if ((now <= id + hint) && (id + hint < nxt))
      return id + hint;
       */
  return select_complement(id);
}

template <size_t P, size_t W>
bool rank_select_few<P, W>::operator[](size_t pos) const {
  //size_t l = lower_bound(pos);
  if (P) {
    //if(l == m_num1) return false;
    return val_a_logi(lower_bound(pos)) == pos;
  } else {
    //if(l == m_num0) return true;
    return val_a_logi(lower_bound(pos)) != pos;
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::rank0(size_t pos) const {
  if (P) {
    /*size_t l = lower_bound(pos);
    return pos - (l < m_num0 ? l : m_num0);*/
    return pos - lower_bound(pos);
  } else {
    return lower_bound(pos);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::rank0(size_t pos, size_t &hint) const {
  if (P) {
    return pos - rank1(pos, hint);
  } else {
    return lower_bound(pos, hint);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::rank1(size_t pos) const {
  if (P) {
    return lower_bound(pos);
  } else {
    return pos - lower_bound(pos);
    /*
    size_t l = lower_bound(pos);
    return pos - (l < m_num1 ? l : m_num1);
     */
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::rank1(size_t pos, size_t &hint) const {
  if (P) {
    return lower_bound(pos, hint);
  } else {
    return pos - rank0(pos, hint);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::select0(size_t id) const {
  if (P) {
    return select_complement(id);
  } else {
    return val_a_logi(id);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::select0(size_t id, size_t &hint) const {
  if (P) {
    return select_complement(id, hint);
  } else {
    return val_a_logi(id);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::select1(size_t id) const {
  if (P) {
    return val_a_logi(id);
  } else {
    return select_complement(id);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::select1(size_t id, size_t &hint) const {
  if (P) {
    return val_a_logi(id);
  } else {
    return select_complement(id, hint);
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::zero_seq_len(size_t pos) const {
  size_t a = lower_bound(pos);
  if (P) {
    if(a == m_num1) return m_num0 + m_num1 - pos;
    return val_a_logi(a) - pos;
  } else {
    if (val_a_logi(a) != pos) return 0;
    size_t cnt, prev, now;
    cnt = 1;
    now = val_a_logi(a);
    while (++a < m_num1 + m_num0) {
      prev = now;
      now = val_a_logi(a);
      if (prev + 1 == now)
        cnt++;
      else
        break;
    }
    return cnt;
  }
}

    template <size_t P, size_t W>
    size_t rank_select_few<P, W>::one_seq_len(size_t pos) const {
      size_t a = lower_bound(pos);
      if (P) {
        if (val_a_logi(a) != pos) return 0;
        size_t cnt, prev, now;
        cnt = 1;
        now = val_a_logi(a);
        while (++a < m_num1 + m_num0) {
          prev = now;
          now = val_a_logi(a);
          if (prev + 1 == now)
            cnt++;
          else
            break;
        }
        return cnt;
      } else {
        if(a == m_num0) return m_num0 + m_num1 - pos;
        return val_a_logi(a) - pos;
      }
    }

template <size_t P, size_t W>
size_t rank_select_few<P, W>::zero_seq_len(size_t pos, size_t &hint) const {
  //dummy
  return zero_seq_len(pos);
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::zero_seq_revlen(size_t pos) const {
  if (!pos)
    return 0;
  if (P) {
    size_t a = lower_bound(pos);
    if(a) return pos - 1 - val_a_logi(a-1);
    else return pos;
  } else {
    size_t a = lower_bound(pos-1);
    if (val_a_logi(a) != pos-1) return 0;
    size_t cnt, last, now;
    cnt = 1;
    now = val_a_logi(a);
    while (--a != 0xFFFFFFFFFFFFFFFF) {
      last = now;
      now = val_a_logi(a);
      if (last == now + 1)
        cnt++;
      else
        break;
    }
    return cnt;
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::zero_seq_revlen(size_t pos, size_t &hint) const {
  return  zero_seq_revlen(pos);
}


template <size_t P, size_t W>
size_t rank_select_few<P, W>::one_seq_len(size_t pos, size_t &hint) const {
  //dummy
  return one_seq_len(pos);
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::one_seq_revlen(size_t pos) const {
  if (!pos)
    return 0;
  if (P) {
    size_t a = lower_bound(pos-1);
    if (val_a_logi(a) != pos-1) return 0;
    size_t cnt, last, now;
    cnt = 1;
    now = val_a_logi(a);
    while (--a != 0xFFFFFFFFFFFFFFFF) {
      last = now;
      now = val_a_logi(a);
      if (last == now + 1)
        cnt++;
      else
        break;
    }
    return cnt;
  } else {
    size_t a = lower_bound(pos);
    if(a) return pos - 1 - val_a_logi(a-1);
    else return pos;
  }
}

template <size_t P, size_t W>
size_t rank_select_few<P, W>::one_seq_revlen(size_t pos, size_t &hint) const {
  return one_seq_revlen(pos);
}

template <size_t P, size_t W>
rank_select_few_builder<P, W>::rank_select_few_builder(size_t num0, size_t num1,
                                                       bool rev) {
  size_t sz = 0;
  size_t cache[8];
  size_t layer = 0;
  size_t n = (P ? num1 : num0);
  while(n){
      sz += n * W;
      cache[layer++] = sz;
      n >>= 8;
  }
  if (!P)
    m_last = rev ? num0 + num1 - 1 : 0;
  assert(layer <= 8);
  size_t align_size = (sz + 7) / 8 * 8;
  m_mempool.resize(align_size + 8 + (2 + layer) * 8);
  uint64_t * base = reinterpret_cast<uint64_t *>(m_mempool.data() + align_size);
  base[0] = m_num0 = num0;
  base[1] = m_num1 = num1;
  m_offset = base + 2;
  memcpy(m_offset, cache, layer * 8);
  base[2 + layer] = m_layer = layer;
  m_it = (m_rev = rev) ? m_mempool.data() + cache[0] - W : m_mempool.data();
}

template <size_t P, size_t W>
void rank_select_few_builder<P, W>::insert(size_t pos) {
  assert(pos <= ~(0xFFFFFFFFULL << (W * 8)));
  if (P) {
    *(reinterpret_cast<uint64_t *>(m_it)) |= pos;
    m_it = m_rev ? m_it - W : m_it + W;
  } else {
    if (m_rev) {
      assert(pos <= m_last);
        while (m_last != pos) {
          *(reinterpret_cast<uint64_t *>(m_it)) |= m_last--;
          m_it -= W;
        }
        m_last--;
    } else {
      assert(pos >= m_last);
        while (m_last != pos) {
          *(reinterpret_cast<uint64_t *>(m_it)) |= m_last++;
          m_it += W;
        }
        m_last++;
    }
  }
}

template <size_t P, size_t W>
void rank_select_few_builder<P, W>::finish(rank_select_few<P, W> *r) {
  if (!P) {
    if (m_rev) {
      while (m_it >= m_mempool.data()) {
        *(reinterpret_cast<uint64_t *>(m_it)) |= m_last--;
        m_it -= W;
      }
    } else {
      while (m_it < m_mempool.data() + m_offset[0]) {
        *(reinterpret_cast<uint64_t *>(m_it)) |= m_last++;
        m_it += W;
      }
    }
  }
  uint8_t *base = m_mempool.data();
  uint8_t *src = base;
  for (int i = 0; i < m_layer - 1; ++i) {
    for (uint8_t *dst = base + m_offset[i]; dst < base + m_offset[i + 1];) {
      memcpy(dst, src, W);
      src += W * 256;
      dst += W;
    }
    src = base + m_offset[i];
  }
  std::swap(m_offset, r->m_offset);
  r->m_layer = m_layer;
  r->m_num0 = m_num0;
  r->m_num1 = m_num1;
  std::swap(m_mempool, r->m_mempool);
  m_it = nullptr;
  m_mempool.clear();
}

template class TERARK_DLL_EXPORT rank_select_few<0, 1>;
template class TERARK_DLL_EXPORT rank_select_few<0, 2>;
template class TERARK_DLL_EXPORT rank_select_few<0, 3>;
template class TERARK_DLL_EXPORT rank_select_few<0, 4>;
template class TERARK_DLL_EXPORT rank_select_few<0, 5>;
template class TERARK_DLL_EXPORT rank_select_few<0, 6>;
template class TERARK_DLL_EXPORT rank_select_few<0, 7>;
template class TERARK_DLL_EXPORT rank_select_few<0, 8>;
template class TERARK_DLL_EXPORT rank_select_few<1, 1>;
template class TERARK_DLL_EXPORT rank_select_few<1, 2>;
template class TERARK_DLL_EXPORT rank_select_few<1, 3>;
template class TERARK_DLL_EXPORT rank_select_few<1, 4>;
template class TERARK_DLL_EXPORT rank_select_few<1, 5>;
template class TERARK_DLL_EXPORT rank_select_few<1, 6>;
template class TERARK_DLL_EXPORT rank_select_few<1, 7>;
template class TERARK_DLL_EXPORT rank_select_few<1, 8>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 1>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 2>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 3>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 4>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 5>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 6>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 7>;
template class TERARK_DLL_EXPORT rank_select_few_builder<0, 8>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 1>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 2>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 3>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 4>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 5>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 6>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 7>;
template class TERARK_DLL_EXPORT rank_select_few_builder<1, 8>;

} // namespace terark