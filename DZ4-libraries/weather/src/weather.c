#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <curl/curl.h>

#include "json.h"

#define FORECAST_JSON "forecast.json"

int usage(int argc)
{
	if (argc > 2)
		printf("Too many arguments!\n");
	
	printf("usage: main [city]\n");
	
	return 1;
}

char* concat(char *s1, char *s2, char *s3)
{
	size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);                      
    size_t len3 = strlen(s3);                      

    char *result = malloc(len1 + len2 + len3 + 1);

    if (!result) {
        fprintf(stderr, "malloc() failed: insufficient memory!\n");
        return NULL;
    }

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    memcpy(result + len1 + len2, s3, len3 + 1);

    return result;
}

int download_file(const char *url) 
{
	CURL *curl;
    FILE *fp;
	CURLcode res;
	char errbuf[CURL_ERROR_SIZE];
	
	curl = curl_easy_init();
	
	if (curl) {
        fp = fopen(FORECAST_JSON,"w+b");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);	// error buff
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);			// timeout
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);			// file availability
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); 			// save to path
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
		fclose(fp);

		if (res == CURLE_OK) {
			// printf("File downloaded successfully!\n");
			return 0;
		} else {
			printf("Downloading error: %s\n", errbuf);
			return 1;
		}
    }
	
	return 1;
}

void forecast(char *city) 
{
	struct json_object *parsed_json;
	struct json_object *parsed_json2;
	struct json_object *parsed_json3;
	struct json_object *current_condition;
	
	struct json_object *weatherDesc;
	struct json_object *weatherDesc_value;
	struct json_object *FeelsLikeC;
	struct json_object *temp_C;
	struct json_object *winddir16Point;
	struct json_object *windspeedKmph;
	
	FILE *fp = fopen(FORECAST_JSON,"r");
	
	parsed_json = json_object_from_file(FORECAST_JSON);
	json_object_object_get_ex(parsed_json, "current_condition", &current_condition);
	parsed_json2 = json_object_array_get_idx(current_condition, 0);
	
	json_object_object_get_ex(parsed_json2, "FeelsLikeC", &FeelsLikeC);
	json_object_object_get_ex(parsed_json2, "temp_C", &temp_C);
	json_object_object_get_ex(parsed_json2, "winddir16Point", &winddir16Point);
	json_object_object_get_ex(parsed_json2, "windspeedKmph", &windspeedKmph);
	
	json_object_object_get_ex(parsed_json2, "weatherDesc", &weatherDesc);
	parsed_json3 = json_object_array_get_idx(weatherDesc, 0);
	json_object_object_get_ex(parsed_json3, "value", &weatherDesc_value);
	
	printf("---------------------------------\n");
	printf(" Current weather in %s\n", city);
	printf(" %s\n", json_object_get_string(weatherDesc_value));
	printf(" %s(%s) C°\n", json_object_get_string(temp_C), json_object_get_string(FeelsLikeC));
	printf(" %s %s Km/h\n", json_object_get_string(winddir16Point), json_object_get_string(windspeedKmph));
    printf("---------------------------------\n"); 	

	fclose(fp);
    remove(FORECAST_JSON);
}

int main (int argc, char * argv[]) 
{
	if (argc != 2){
		return usage(argc);
	}
	
	char *url1 = "https://wttr.in/";
	char *url2 = "?format=j1";
	char *city = argv[1];
	char *url = concat(url1, city, url2);

	download_file(url);
	forecast(city);
	
	free(url);
	return 0;
}


