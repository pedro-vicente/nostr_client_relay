#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif

#include "nostri.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// try_subcommand
/////////////////////////////////////////////////////////////////////////////////////////////////////

static void try_subcommand(int argc, const char* argv[])
{
  static char buf[128] = { 0 };
  const char* sub = argv[1];
  if (strlen(sub) >= 1 && sub[0] != '-')
  {
    snprintf(buf, sizeof(buf) - 1, "nostril-%s", sub);
    execvp(buf, (char* const*)argv + 1);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// the one and only main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
  struct args args = { 0 };
  struct nostr_event ev = { 0 };
  struct key key;
  secp256k1_context* ctx;

  if (argc < 2)
    usage();

  if (!init_secp_context(&ctx))
    return 2;

  try_subcommand(argc, argv);

  if (!parse_args(argc, argv, &args, &ev))
  {
    usage();
    return 10;
  }

  if (args.tags)
  {
    ev.explicit_tags = args.tags;
  }

  make_event_from_args(&ev, &args);

  if (args.sec)
  {
    if (!decode_key(ctx, args.sec, &key))
    {
      return 8;
    }
  }
  else
  {
    int* difficulty = NULL;
    if ((args.flags & HAS_DIFFICULTY) && (args.flags & HAS_MINE_PUBKEY))
    {
      difficulty = &args.difficulty;
    }

    if (!generate_key(ctx, &key, difficulty))
    {
      fprintf(stderr, "could not generate key\n");
      return 4;
    }
    fprintf(stderr, "secret_key ");
    print_hex(key.secret, sizeof(key.secret));
    fprintf(stderr, "\n");
  }

  if (args.flags & HAS_ENCRYPT)
  {
    int kind = args.flags & HAS_KIND ? args.kind : 4;
    if (!make_encrypted_dm(ctx, &key, &ev, args.encrypt_to, kind))
    {
      fprintf(stderr, "error making encrypted dm\n");
      return 0;
    }
  }

  // set the event's pubkey
  memcpy(ev.pubkey, key.pubkey, 32);

  if (args.flags & HAS_DIFFICULTY && !(args.flags & HAS_MINE_PUBKEY))
  {
    if (!mine_event(&ev, args.difficulty))
    {
      fprintf(stderr, "error when mining id\n");
      return 22;
    }
  }
  else
  {
    if (!generate_event_id(&ev))
    {
      fprintf(stderr, "could not generate event id\n");
      return 5;
    }
  }

  if (!sign_event(ctx, &key, &ev))
  {
    fprintf(stderr, "could not sign event\n");
    return 6;
  }

  char* json = malloc(102400);
  if (!print_event(&ev, args.flags & HAS_ENVELOPE, &json))
  {
    fprintf(stderr, "buffer too small\n");
    return 88;
  }

  fprintf(stderr, "%s", json);
  return 0;
}

