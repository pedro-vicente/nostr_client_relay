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

int parse_args(int argc, const char* argv[], struct args* args, struct nostr_event* ev);

void usage()
{
  printf("usage: nostril [OPTIONS]\n");
  printf("\n");
  printf("  OPTIONS\n");
  printf("\n");
  printf("      --content <string>              the content of the note\n");
  printf("      --dm <hex pubkey>               make an encrypted dm to said pubkey. sets kind and tags.\n");
  printf("      --envelope                      wrap in [\"EVENT\",...] for easy relaying\n");
  printf("      --kind <number>                 set kind\n");
  printf("      --created-at <unix timestamp>   set a specific created-at time\n");
  printf("      --sec <hex seckey>              set the secret key for signing, otherwise one will be randomly generated\n");
  printf("      --pow <difficulty>              number of leading 0 bits of the id to mine\n");
  printf("      --mine-pubkey                   mine a pubkey instead of id\n");
  printf("      --tag <key> <value>             add a tag\n");
  printf("      -e <event_id>                   shorthand for --tag e <event_id>\n");
  printf("      -p <pubkey>                     shorthand for --tag p <pubkey>\n");
  printf("      -t <hashtag>                    shorthand for --tag t <hashtag>\n");
  exit(1);
}

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
// main
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
  if (!print_event(&ev, &json))
  {
    fprintf(stderr, "buffer too small\n");
    return 88;
  }

  fprintf(stderr, "%s", json);
  return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_args
// new arguments are
// --uri <URI>
// --req <evend_id>
// --rand
/////////////////////////////////////////////////////////////////////////////////////////////////////

int parse_args(int argc, const char* argv[], struct args* args, struct nostr_event* ev)
{
  const char* arg, * arg2;
  uint64_t n;
  int has_added_tags = 0;

  argv++; argc--;
  for (; argc; )
  {
    arg = *argv++; argc--;

    if (!strcmp(arg, "--help"))
    {
      usage();
    }

    if (!argc)
    {
      fprintf(stderr, "expected argument: '%s'\n", arg);
      return 0;
    }

    else if (!strcmp(arg, "--sec"))
    {
      args->sec = *argv++; argc--;
    }
    else if (!strcmp(arg, "--created-at"))
    {
      arg = *argv++; argc--;
      if (!parse_num(arg, &args->created_at))
      {
        fprintf(stderr, "created-at must be a unix timestamp\n");
        return 0;
      }
      else
      {
        args->flags |= HAS_CREATED_AT;
      }
    }
    else if (!strcmp(arg, "--kind"))
    {
      arg = *argv++; argc--;
      if (!parse_num(arg, &n))
      {
        fprintf(stderr, "kind should be a number, got '%s'\n", arg);
        return 0;
      }
      args->kind = (int)n;
      args->flags |= HAS_KIND;
    }
    else if (!strcmp(arg, "--envelope"))
    {
      args->flags |= HAS_ENVELOPE;
    }
    else if (!strcmp(arg, "--tags"))
    {
      if (args->flags & HAS_DIFFICULTY)
      {
        fprintf(stderr, "can't combine --tags and --pow (yet)\n");
        return 0;
      }
      if (has_added_tags)
      {
        fprintf(stderr, "can't combine --tags and --tag (yet)");
        return 0;
      }
      arg = *argv++; argc--;
      args->tags = arg;
    }
    else if (!strcmp(arg, "-e"))
    {
      has_added_tags = 1;
      arg = *argv++; argc--;
      if (!nostr_add_tag(ev, "e", arg))
      {
        fprintf(stderr, "couldn't add e tag");
        return 0;
      }
    }
    else if (!strcmp(arg, "-p"))
    {
      has_added_tags = 1;
      arg = *argv++; argc--;
      if (!nostr_add_tag(ev, "p", arg))
      {
        fprintf(stderr, "couldn't add p tag");
        return 0;
      }
    }
    else if (!strcmp(arg, "-t"))
    {
      has_added_tags = 1;
      arg = *argv++; argc--;
      if (!nostr_add_tag(ev, "t", arg))
      {
        fprintf(stderr, "couldn't add t tag");
        return 0;
      }
    }
    else if (!strcmp(arg, "--tag"))
    {
      has_added_tags = 1;
      if (args->tags)
      {
        fprintf(stderr, "can't combine --tag and --tags (yet)");
        return 0;
      }
      arg = *argv++; argc--;
      if (argc == 0)
      {
        fprintf(stderr, "expected two arguments to --tag\n");
        return 0;
      }
      arg2 = *argv++; argc--;
      if (!nostr_add_tag(ev, arg, arg2))
      {
        fprintf(stderr, "couldn't add tag '%s' '%s'\n", arg, arg2);
        return 0;
      }
    }
    else if (!strcmp(arg, "--mine-pubkey"))
    {
      args->flags |= HAS_MINE_PUBKEY;
    }
    else if (!strcmp(arg, "--pow"))
    {
      if (args->tags)
      {
        fprintf(stderr, "can't combine --tags and --pow (yet)\n");
        return 0;
      }
      arg = *argv++; argc--;
      if (!parse_num(arg, &n))
      {
        fprintf(stderr, "could not parse difficulty as number: '%s'\n", arg);
        return 0;
      }
      args->difficulty = n;
      args->flags |= HAS_DIFFICULTY;
    }
    else if (!strcmp(arg, "--dm"))
    {
      arg = *argv++; argc--;
      if (!hex_decode(arg, strlen(arg), args->encrypt_to, 32))
      {
        fprintf(stderr, "could not decode encrypt-to pubkey");
        return 0;
      }
      args->flags |= HAS_ENCRYPT;
    }
    else if (!strcmp(arg, "--content"))
    {
      arg = *argv++; argc--;
      args->content = arg;
    }
    else
    {
      fprintf(stderr, "unexpected argument '%s'\n", arg);
      return 0;
    }
  }

  if (!args->content)
    args->content = "";

  return 1;
}
