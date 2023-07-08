#ifndef _NOSTRI_H_
#define _NOSTRI_H_

#include "secp256k1.h"
#include "secp256k1_ecdh.h"
#include "secp256k1_schnorrsig.h"

#define VERSION "0.1.3"
#define MAX_TAGS 32
#define MAX_TAG_ELEMS 16
#define HAS_CREATED_AT (1<<1)
#define HAS_KIND (1<<2)
#define HAS_ENVELOPE (1<<3)
#define HAS_ENCRYPT (1<<4)
#define HAS_DIFFICULTY (1<<5)
#define HAS_MINE_PUBKEY (1<<6)

/////////////////////////////////////////////////////////////////////////////////////////////////////
// key
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct key
{
  secp256k1_keypair pair;
  unsigned char secret[32];
  unsigned char pubkey[32];
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// args
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct args 
{
  unsigned int flags;
  int kind;
  int difficulty;
  unsigned char encrypt_to[32];
  const char* sec;
  const char* tags;
  const char* content;
  uint64_t created_at;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// nostr_tag
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct nostr_tag
{
  const char* strs[MAX_TAG_ELEMS];
  int num_elems;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// nostr_event
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct nostr_event
{
  unsigned char id[32];
  unsigned char pubkey[32];
  unsigned char sig[64];
  const char* content;
  uint64_t created_at;
  int kind;
  const char* explicit_tags;
  struct nostr_tag tags[MAX_TAGS];
  int num_tags;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// public API
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif 
  int parse_num(const char* arg, uint64_t* t);
  int nostr_add_tag(struct nostr_event* ev, const char* t1, const char* t2);
  int hex_decode(const char* str, size_t slen, void* buf, size_t bufsize);
  int init_secp_context(secp256k1_context** ctx);
  void make_event_from_args(struct nostr_event* ev, struct args* args);
  int decode_key(secp256k1_context* ctx, const char* secstr, struct key* key);
  int generate_key(secp256k1_context* ctx, struct key* key, int* difficulty);
  void print_hex(unsigned char* data, size_t size);
  int make_encrypted_dm(secp256k1_context* ctx, struct key* key, struct nostr_event* ev, unsigned char nostr_pubkey[32], int kind);
  int mine_event(struct nostr_event* ev, int difficulty);
  int generate_event_id(struct nostr_event* ev);
  int sign_event(secp256k1_context* ctx, struct key* key, struct nostr_event* ev);
  int print_event(struct nostr_event* ev, char** json);

  int make_message(struct args* args, struct nostr_event* ev, char** json);
#ifdef __cplusplus
}
#endif


#endif