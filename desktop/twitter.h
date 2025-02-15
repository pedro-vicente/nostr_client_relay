#ifndef TWITTER_NOSTR__H
#define TWITTER_NOSTR__H

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////
// Tweet
// a "tweet" is a message on the network 
// it has text, images, media, a list of responses, list of likes, retweets
///////////////////////////////////////////////////////////////////////////////////////

class Tweet
{
public:
  Tweet() : likes(0) {}
  std::string username;
  std::string content;
  size_t likes;
  std::vector<std::string> responses;
};

///////////////////////////////////////////////////////////////////////////////////////
// User
// a user has a name and a list of messages posted 
// he/she can reply, like, retweet
///////////////////////////////////////////////////////////////////////////////////////

class User
{
public:
  User(std::string u) : username(u) {}
  std::string username;
  std::vector<Tweet> tweets;

  //assign ownership, but do not store (will be done by Twitter)
  void make_tweet(Tweet& tweet, const std::string& content)
  {
    tweet.username = username;
    tweet.content = content;
  }

  void reply_to_tweet(Tweet& tweet, std::string response)
  {
    tweet.responses.push_back(response);
  }

  void like_tweet(Tweet& tweet)
  {
    tweet.likes++;
  }

  void retweet(Tweet& tweet)
  {
    std::string response = "RT by @" + username + ": " + tweet.content;
    tweet.responses.push_back(response);
  }
};

///////////////////////////////////////////////////////////////////////////////////////
// Twitter
// storage for users;
// context is application memory only, i.e loaded messages (from cache, network)
// actions are: 
// 1) post a message by a current user
// 2) add a user
///////////////////////////////////////////////////////////////////////////////////////

class Twitter
{
public:
  Twitter() {}
  std::vector<User> users;

  User* find_user(const std::string& username)
  {
    for (auto& user : users)
    {
      if (user.username == username)
      {
        return &user;
      }
    }
    return nullptr;
  }

  void post_tweet(const Tweet& tweet)
  {
    User* user = find_user(tweet.username);
    if (user)
    {
      user->tweets.push_back(tweet);
    }
    else
    {
    }
  }
};


#endif
