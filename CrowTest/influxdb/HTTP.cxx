﻿#include "HTTP.h"
#include <iostream>

namespace influxdb
{
namespace transports
{

HTTP::HTTP(const std::string& url)
{
  initCurl(url);
  initCurlRead(url);
}

void HTTP::initCurl(const std::string& url)
{
  CURLcode globalInitResult = curl_global_init(CURL_GLOBAL_ALL);
  if (globalInitResult != CURLE_OK) {
    throw std::runtime_error(std::string("cURL init") + curl_easy_strerror(globalInitResult));
  }

  std::string writeUrl = url;
  auto position = writeUrl.find("?");
  if (writeUrl.at(position - 1) != '/') {
    writeUrl.insert(position, "/write");
  } else {
    writeUrl.insert(position, "write");
  }
  writeHandle = curl_easy_init();
  curl_easy_setopt(writeHandle, CURLOPT_URL,  writeUrl.c_str());
  curl_easy_setopt(writeHandle, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(writeHandle, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(writeHandle, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(writeHandle, CURLOPT_POST, 1);
  curl_easy_setopt(writeHandle, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(writeHandle, CURLOPT_TCP_KEEPINTVL, 60L);
  FILE *devnull = fopen("/dev/null", "w+");
  curl_easy_setopt(writeHandle, CURLOPT_WRITEDATA, devnull);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void HTTP::initCurlRead(const std::string& url)
{
  mReadUrl = url + "&q=";
  auto position = mReadUrl.find("?");
  if (mReadUrl.at(position - 1) != '/') {
    mReadUrl.insert(position, "/query");
  } else {
    mReadUrl.insert(position, "query");
  }
  readHandle = curl_easy_init();
  curl_easy_setopt(readHandle, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(readHandle, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(readHandle, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(readHandle, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(readHandle, CURLOPT_TCP_KEEPINTVL, 60L);
  curl_easy_setopt(readHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
}

std::string HTTP::query(const std::string& query)
{
  CURLcode response;
  std::string buffer;
#ifdef DEBUG_OUTPUT
  std::cout << "start http query!" <<std::endl;
#endif
  char* encodedQuery = curl_easy_escape(readHandle, query.c_str(), query.size());
  auto fullUrl = mReadUrl + std::string(encodedQuery);
#ifdef DEBUG_OUTPUT
  std::cout << "full url : " << fullUrl<< std::endl;
#endif
  curl_easy_setopt(readHandle, CURLOPT_URL, fullUrl.c_str());
  curl_easy_setopt(readHandle, CURLOPT_WRITEDATA, &buffer);
#ifdef DEBUG_OUTPUT
  std::cout << "get buffer : " << buffer <<std::endl;
#endif
  response = curl_easy_perform(readHandle);
  if (response != CURLE_OK) {
    throw std::runtime_error(curl_easy_strerror(response));
  }
  return buffer;
}

void HTTP::enableBasicAuth(const std::string& auth)
{
  curl_easy_setopt(writeHandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  curl_easy_setopt(writeHandle, CURLOPT_USERPWD, auth.c_str());
  curl_easy_setopt(readHandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  curl_easy_setopt(readHandle, CURLOPT_USERPWD, auth.c_str());
}

void HTTP::enableSsl()
{
  curl_easy_setopt(readHandle, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(writeHandle, CURLOPT_SSL_VERIFYPEER, 0L);
}

HTTP::~HTTP()
{
  curl_easy_cleanup(writeHandle);
  curl_easy_cleanup(readHandle);
  curl_global_cleanup();
}

void HTTP::send(std::string&& post)
{
  CURLcode response;
  long responseCode;
#ifdef DEBUG_OUTPUT
  std::cout << "post string : " << post << std::endl;
#endif
  curl_easy_setopt(writeHandle, CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(writeHandle, CURLOPT_POSTFIELDSIZE, (long) post.length());
  response = curl_easy_perform(writeHandle);
  curl_easy_getinfo(writeHandle, CURLINFO_RESPONSE_CODE, &responseCode);
  if (response != CURLE_OK) {
    throw std::runtime_error(curl_easy_strerror(response));
  }
  if (responseCode < 200 || responseCode > 206) {
    throw std::runtime_error("Response code : " + std::to_string(responseCode));
  }
}

} // namespace transports
} // namespace influxdb

