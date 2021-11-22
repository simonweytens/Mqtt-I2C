#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS "tcp://raspberrypi.local:1883"
#define CLIENTID "ExampleClientsub"
#define TOPIC "ArnoSimon"
#define PAYLOAD "hello world"
#define QOS 1
#define TIMEOUT 10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void makeDatabase(void)
{
    MYSQL *con = mysql_init(NULL);
    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "root", "raspberry",
                           NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    if (mysql_query(con, "CREATE DATABASE DbArnoSimon"))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
    }
    if (mysql_query(con, "USE DbArnoSimon"))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
    }

    if (mysql_query(con, "CREATE TABLE TCval(id INT NOT NULL AUTO_INCREMENT, PRIMARY KEY (id), Temp INT, date_time datetime NOT NULL DEFAULT CURRENT_TIMESTAMP)"))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return;
    }

    mysql_close(con);
    printf("db succed! \n");
    return;
}

void updateDatabase(int Temp)
{
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		printf("MySQL initialization failed");
		return;
	}
	if (mysql_real_connect(con, "localhost", "root", "raspberry",
						   "DbArnoSimon", 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return;
	}

	char buffer[100];
	snprintf(buffer, sizeof(buffer), "insert into TCval(Temp) values( %i)", Temp);

	if (mysql_query(con, buffer))
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return;
	}
	mysql_close(con);
	return;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payloadptr = message->payload;
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    int Temp = strtol(payloadptr,NULL,10);

    if (Temp)updateDatabase(Temp);
    else printf("message has to be an int\n"); 
    
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char *argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    makeDatabase();
    MQTTClient_create(&client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
           TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do
    {
        ch = getchar();
    } while (ch != 'Q' && ch != 'q');


    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}