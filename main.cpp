#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct ListNode {
  ListNode *prev{nullptr};
  ListNode *next{nullptr};
  ListNode *rand{nullptr};
  std::string data;
};

void set_rand(ListNode *head, size_t pos, size_t rand_pos) {
  ListNode *pointer = head;

  for (size_t curr = 1; curr < pos; ++curr) {
    pointer = pointer->next;
  }
  for (size_t curr = 1; curr < rand_pos; ++curr) {
    head = head->next;
  }
  pointer->rand = head;
}

void serialize(ListNode *head, const std::string &out_path,
               std::vector<size_t> &rands) {
  std::ofstream out_stream(out_path, std::ios::binary);
  if (!out_stream.is_open())
    return;
  size_t n = rands.size();

  out_stream.write(reinterpret_cast<const char *>(&n), sizeof(n));

  ListNode *curr_node = head;
  for (size_t i = 0; i < n && curr_node; ++i) {
    size_t prev_idx = i;
    size_t next_idx = (i == n - 1) ? 0 : i + 2;
    size_t rand_idx = rands[i];

    out_stream.write(reinterpret_cast<const char *>(&prev_idx),
                     sizeof(prev_idx));
    out_stream.write(reinterpret_cast<const char *>(&next_idx),
                     sizeof(next_idx));
    out_stream.write(reinterpret_cast<const char *>(&rand_idx),
                     sizeof(rand_idx));

    size_t data_len = curr_node->data.length();
    out_stream.write(reinterpret_cast<const char *>(&data_len),
                     sizeof(data_len));
    out_stream.write(curr_node->data.c_str(), data_len);

    curr_node = curr_node->next;
  }
}

int main() {
  std::string in_path = "inlet.in";
  std::string out_path = "outlet.out";
  std::vector<size_t> rands;
  std::ifstream in_stream(in_path);
  if (!in_stream.is_open())
    return 1;
  ListNode head;
  ListNode *curr = &head;
  std::string line;
  getline(in_stream, line);
  size_t del = line.find('&');
  if (del != std::string::npos) {
    head.data = line.substr(0, del);
    int32_t rand = stoi(line.substr(del + 1));
    rands.push_back(rand);
  }

  while (getline(in_stream, line)) {
    auto temp = new ListNode();
    curr->next = temp;
    temp->prev = curr;
    size_t del = line.find('&');
    if (del != std::string::npos) {
      temp->data = line.substr(0, del);
      int32_t rand = stoi(line.substr(del + 1));
      rands.push_back(rand);
      curr = temp;
    }
  }
  for (size_t i = 0; i < rands.size(); ++i) {
    set_rand(&head, i + 1, rands[i]);
  }
  serialize(&head, out_path, rands);
}
