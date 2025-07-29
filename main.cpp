#define DISCORDPP_IMPLEMENTATION
#include "discordpp.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <csignal>
#include "crow_all.h"

// Replace with your Discord Application ID
const uint64_t APPLICATION_ID = 1398894706611458221;

// Create a flag to stop the application
std::atomic<bool> running = true;

// Signal handler to stop the application
void signalHandler(int signum) {
  running.store(false);
}

discordpp::Activity updatePresence(const std::string state, const std::string details, const std::string largeImage, const std::string largeText, const std::string smallImage, const std::string smallText) {
    discordpp::Activity activity;
    activity.SetType(discordpp::ActivityTypes::Playing);
    activity.SetState(state);
    activity.SetDetails(details);

    discordpp::ActivityAssets assets;
    assets.SetLargeImage(largeImage);
    assets.SetLargeText(largeText);
    assets.SetSmallImage(smallImage);
    assets.SetSmallText(smallText);
    activity.SetAssets(assets);

    return activity;
}

int main() {
  std::signal(SIGINT, signalHandler);
  std::cout << "🚀 Initializing Discord SDK...\n";

  // Create our Discord Client
  auto client = std::make_shared<discordpp::Client>();

  client->AddLogCallback([](auto message, auto severity) {
      std::cout << "[" << EnumToString(severity) << "] " << message << std::endl;
  }, discordpp::LoggingSeverity::Info);

  client->SetStatusChangedCallback([client](discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail) {
      std::cout << "🔄 Status changed: " << discordpp::Client::StatusToString(status) << std::endl;

      if (status == discordpp::Client::Status::Ready) {
          std::cout << "✅ Client is ready! You can now call SDK functions.\n";

          // Access initial relationships data
          std::cout << "👥 Friends Count: " << client->GetRelationships().size() << std::endl;

          // Configure rich presence details
          discordpp::Activity activity = updatePresence(
              "Experience: Presence Plugin",
              "Editing DiscordPresence",
              "script_light_icon",
              "Editing a Script",
              "roblox-studio-icon-filled-256",
              "Roblox Studio"
          );
          client->UpdateRichPresence(activity, {});
          
          //// Crow
          crow::SimpleApp app;

          CROW_ROUTE(app, "/updateRobloxPresence").methods("POST"_method)
              ([client](const crow::request& req) {
              try {
                  auto body = crow::json::load(req.body);
                  if (!body) {
                      return crow::response(400, "Invalid JSON");
                  }


                  std::cout << "Body Size: " << body.keys().size() << std::endl;

                  std::string state = body["state"].s();
                  std::string details = body["details"].s();
                  std::string largeImage = body["largeImage"].s();
                  std::string largeText = body["largeText"].s();
                  std::string smallImage = body["smallImage"].s();
                  std::string smallText = body["smallText"].s();

                  discordpp::Activity activity = updatePresence(state, details, largeImage, largeText, smallImage, smallText);
                  client->UpdateRichPresence(activity, {});
                  return crow::response(200, "Presence Updated");
              }
              catch (const std::exception& e) {
                  return crow::response(500, e.what());
              }
          });

          std::cout << "Listening on http://localhost:3000\n";
          app.port(3000).multithreaded().run();
      }
      else if (error != discordpp::Client::Error::None) {
          std::cerr << "❌ Connection Error: " << discordpp::Client::ErrorToString(error) << " - Details: " << errorDetail << std::endl;
      }
  });

  // Generate OAuth2 code verifier for authentication
  auto codeVerifier = client->CreateAuthorizationCodeVerifier();

  // Set up authentication arguments
  discordpp::AuthorizationArgs args{};
  args.SetClientId(APPLICATION_ID);
  args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
  args.SetCodeChallenge(codeVerifier.Challenge());

  // Begin authentication process
  client->Authorize(args, [client, codeVerifier](auto result, auto code, auto redirectUri) {
      if (!result.Successful()) {
          std::cerr << "❌ Authentication Error: " << result.Error() << std::endl;
          return;
      }
      else {
          std::cout << "✅ Authorization successful! Getting access token...\n";

          // Exchange auth code for access token
          client->GetToken(APPLICATION_ID, code, codeVerifier.Verifier(), redirectUri,
              [client](discordpp::ClientResult result,
                  std::string accessToken,
                  std::string refreshToken,
                  discordpp::AuthorizationTokenType tokenType,
                  int32_t expiresIn,
                  std::string scope) {
                      std::cout << "🔓 Access token received! Establishing connection...\n";
                      // Next Step: Update the token and connect
                      client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [client](discordpp::ClientResult result) {
                          if (result.Successful()) {
                              std::cout << "🔑 Token updated, connecting to Discord...\n";
                              client->Connect();
                          }
                      });
              });
      }
  });


  // Keep application running to allow SDK to receive events and callbacks
  while (running) {
      discordpp::RunCallbacks();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
