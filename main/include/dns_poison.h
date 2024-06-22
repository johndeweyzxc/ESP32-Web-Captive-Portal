#pragma once
#include <stdint.h>

typedef struct {
  uint16_t transaction_id;
  uint16_t flags;
  uint16_t questions;
  uint16_t answer_rrs;
  uint16_t authority_rrs;
  uint16_t additional_rrs;
  uint8_t queries_and_answers[100];
} dns_query_t;

typedef struct __attribute__((packed)) {
  uint16_t name;
  uint16_t type;
  uint16_t dns_class;
  uint32_t ttl;
  uint16_t data_len;
  uint8_t address[4];
} dns_query_answer_t;

#define TASK_DNS_POISON_NAME "DNS_POISON"
#define TASK_DNS_POISON_STACK_SIZE 4096
#define TASK_DNS_POISON_PRIORITY 2
#define TASK_DNS_POISON_CORE_TO_USE 1

void dns_poison_create();
