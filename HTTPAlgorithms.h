#include <curl/curl.h>
#include <string>

using namespace std;

string RetrieveWebpage(string url)
{
	CURL * curl;
	CURLcode res;
	string output;

	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
	}

	return output;
}
