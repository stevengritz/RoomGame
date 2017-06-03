// File for playing the adventure game with room files

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char FILE_NAME1[256] = "Room 1";
char FILE_NAME2[256] = "Room 2";
char FILE_NAME3[256] = "Room 3";
char FILE_NAME4[256] = "Room 4";
char FILE_NAME5[256] = "Room 5";
char FILE_NAME6[256] = "Room 6";
char FILE_NAME7[256] = "Room 7";

char* FILE_NAMES[] = { FILE_NAME1, FILE_NAME2 , FILE_NAME3 , FILE_NAME4 , FILE_NAME5 , FILE_NAME6 , FILE_NAME7 };
int NUMBER_OF_ROOM_FILES = 7;

typedef struct Room Room;
typedef enum Room_Type RoomType;

enum Room_Type
{
	START_ROOM = 0,
	MID_ROOM = 1,
	END_ROOM = 2
};

char* roomTypeString(RoomType type)
{
	switch (type)
	{
	case START_ROOM:
		return "START_ROOM";
		break;
	case MID_ROOM:
		return "MID_ROOM";
		break;
	case END_ROOM:
		return "END_ROOM";
		break;
	default:
		return "INVALID ROOM TYPE";
		break;
	}
	return "ERROR";
}



struct Room
{

	int room_num;
	char name[256];
	int num_connections;
	RoomType type;
	char connection_names[6][256];
	int room_numbers[6];

};

char* stripLeadingSpace(char* string)
{
	if (!string)
		return string;
	char *end;

	// Trim leading space
	while (isspace((unsigned char)*string)) string++;

	if (*string == 0)  // All spaces?
		return string;

	// Trim trailing space
	end = string + strlen(string) - 1;
	while (end > string && isspace((unsigned char)*end)) end--;

	// Write new null terminator
	*(end + 1) = 0;

	return string;
}

Room** readRoomsFromFiles(int num_rooms)
{
	Room** rooms = malloc(sizeof(Room*) * num_rooms);
	int room_array_index;
	for (room_array_index = 0; room_array_index < num_rooms; room_array_index++)
		rooms[room_array_index] = (Room*)malloc(sizeof(Room));

	FILE* roomFile;
	char buffer[256];

	int file_index;
	for (file_index = 0; file_index < NUMBER_OF_ROOM_FILES; file_index++)
	{
		int connection_count = 0;
		rooms[file_index]->num_connections = 0;
		roomFile = fopen(FILE_NAMES[file_index], "r");
		//while loop for reading file
		while (!feof(roomFile))
		{

			fgets(buffer, 256, roomFile);
			char* token = strtok(buffer, ":");
			char* pos = NULL;
			if (strcmp(token, "ROOM NAME") == 0)
			{
				token = strtok(NULL, ":");
				token = stripLeadingSpace(token);
				strcpy(rooms[file_index]->name, token);
			}
			pos = strstr(token, "CONNECTION");
			if (pos)
			{
				token = strtok(NULL, ":");
				token = stripLeadingSpace(token);
				strcpy(rooms[file_index]->connection_names[connection_count], token);
				rooms[file_index]->num_connections++;
				connection_count++;
			}

			if (strcmp(token, "ROOM TYPE") == 0)
			{
				token = strtok(NULL, ":");
				token = stripLeadingSpace(token);
				if (strcmp(token, "START_ROOM") == 0)
					rooms[file_index]->type = START_ROOM;
				if (strcmp(token, "MID_ROOM") == 0)
					rooms[file_index]->type = MID_ROOM;
				if (strcmp(token, "END_ROOM") == 0)
					rooms[file_index]->type = END_ROOM;
				//strcpy(rooms[file_index]->type, token);
				fgets(buffer, 256, roomFile); // Kind of hackish, but forces the eof
			}
			
		}

		fclose(roomFile);
	}
	return rooms;
}

int roomListNameMap(Room* rooms[], char* name)
{
	int room_index;
	for (room_index = 0; room_index < 7; room_index++)
	{
		if (strcmp(rooms[room_index]->name, name) == 0)
			return room_index;
	}
	return -1; // name not found
}

int main(int argc, char* argv[])
{
	// Initialize rooms list
	Room** room_list;
	room_list = readRoomsFromFiles(7);

	RoomType type;
	int current_room = -1;
	int room_index;
	int sub_index;
	int num_steps = 0;
	char path_taken[50][256];
	for (room_index = 0; room_index < 7; room_index++)
	{
		type = room_list[room_index]->type;

		switch (type)	
		{
		case START_ROOM:
			current_room = room_index;
			break;
		default:
			break;
		}
		if (current_room >= 0)
			break;
	}
	// Found the starting room, current_room
	int game_state = 1;
	while (game_state)
	{
		// Start loopable
		printf("\nCURRENT LOCATION: %s\n", room_list[current_room]->name);
		printf("POSSIBLE CONNECTIONS: ");
		for (room_index = 0; room_index < room_list[current_room]->num_connections; room_index++)
		{
			if (room_index != room_list[current_room]->num_connections - 1)
				printf("%s, ", room_list[current_room]->connection_names[room_index]);
			else
				printf("%s.\n", room_list[current_room]->connection_names[room_index]);
		}
		printf("WHERE TO? ");
		char current_player_selection[256];
		gets(current_player_selection);

		// Look through connections
		int selected_index = roomListNameMap(room_list, current_player_selection);
		if (selected_index < 0)
		{
			printf("\nHUH ? I DON’T UNDERSTAND THAT ROOM.TRY AGAIN.\n");
			continue;
		}
		else
		{
			strcpy(path_taken[num_steps], current_player_selection);
			num_steps++;
			current_room = selected_index;
		}


		type = room_list[current_room]->type;
		if (type == END_ROOM)
		{
			game_state = 0;
			printf("\n\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!");
			printf("\nYOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", num_steps);
			int step;
			for (step = 0; step < num_steps; step++)
			{
				printf("%s\n", path_taken[step]);
			}
		}

	}
	// loopable


	return 0;
}