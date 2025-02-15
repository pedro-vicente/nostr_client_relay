#include "twitter.h"
#include <iostream>
#include <sstream>

void display_tweet(const Tweet& t);
void display_user_tweets(Twitter& twitter, const std::string& username);

///////////////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////////////

int main()
{
  Twitter twitter;

  // create users
  User user1("Alice");
  User user2("Bob");

  // add existing users
  twitter.users.push_back(user1);
  twitter.users.push_back(user2);

  // create a tweet
  Tweet tweet;

  //user take ownership 
  user1.make_tweet(tweet, "Tweet #1");

  // users interact with the tweet
  user2.reply_to_tweet(tweet, "Response #1");

  //like
  user2.like_tweet(tweet);

  //retweet
  user2.retweet(tweet);

  //add to storage
  twitter.post_tweet(tweet);

  // Posting tweets
  for (int idx = 0; idx < 2; idx++)
  {
    std::stringstream s;
    s << "Tweet # " << idx + 2;
    Tweet tweet;
    user1.make_tweet(tweet, s.str());

    //add to storage
    twitter.post_tweet(tweet);
  }

  // Displaying tweets
  display_user_tweets(twitter, "Alice");
  display_user_tweets(twitter, "Bob");

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
// display_user_tweets
///////////////////////////////////////////////////////////////////////////////////////

void display_user_tweets(Twitter& twitter, const std::string& username)
{
  User* user = twitter.find_user(username);
  if (user)
  {
    std::cout << "Tweets by @" << username << ":" << std::endl;
    for (const auto& tweet : user->tweets)
    {
      display_tweet(tweet);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////
// display_user_tweets
///////////////////////////////////////////////////////////////////////////////////////

void display_tweet(const Tweet& t)
{
  std::cout << "Tweet by @" << t.username << ": " << t.content << std::endl;
  std::cout << "Likes: " << t.likes << std::endl;
  std::cout << "Responses:" << std::endl;
  for (const auto& response : t.responses)
  {
    std::cout << " - " << response << std::endl;
  }
}