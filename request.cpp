#include <iostream>
#include <thread>
#include <vector>
#include <curl/curl.h>
#include <mutex>
#include <chrono>

std::mutex print_mutex;

void send_request(int i, const std::string& url) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        CURLcode res = curl_easy_perform(curl);

        std::lock_guard<std::mutex> lock(print_mutex);
        if (res == CURLE_OK) {
            std::cout << "Richiesta " << i << ": OK\n";
        } else {
            std::cout << "Richiesta " << i << ": Errore - " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
}

int main() {
    const std::string url = "https://sitelink.com";
    const int total_requests = 10000;// Total number of requests to send
    const int max_threads = 100; // Maximum number of concurrent threads depends on system resources

    std::vector<std::thread> threads;
    int i = 0;

    while (i < total_requests) {
        for (int t = 0; t < max_threads && i < total_requests; ++t, ++i) {
            threads.emplace_back(send_request, i, url);
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        threads.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Optional delay to avoid overwhelming the server
    }

    return 0;
}
