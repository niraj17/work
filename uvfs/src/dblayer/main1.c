#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
    /* MYSQL data*/
    MYSQL conn; /*Represents a connection instance */
    MYSQL_RES * res; /* Represents the resultset of the last executed query */
	MYSQL_ROW row; /* Represents one row of the result set */
    my_ulonglong num_rows = 0; /*Number of rows in the result set */
    int num_fields = 0;

    char * server = "localhost";
    char * username = "lvaikunt";
    char * password = "sid2deb4.ux";
    char * database = "auditdb";
    char * query = "select * from deptable";
    my_init();
    mysql_init(&conn);
    // mysql_real_connect returns MYSQL* , check for NULL here
    if(mysql_real_connect(&conn, server, username, password, database,
                           0, 
                           NULL, 
                            0 )   == NULL)
    {
        // Error in connecting to database
        fprintf(stderr, "%s\n", mysql_error(&conn));
        exit(1);
    }
    
    // Send the select * query 
    if( mysql_query (&conn, query))
    {
        // Error in executing query 
        fprintf(stderr, "%s\n", mysql_error(&conn));
        exit(1);
    }

    res = mysql_store_result(&conn);
    
    // Res can be null if the query did not return a resultset or if it encountered
     // an error 
     //
    if(res == NULL)
    {
        // There are other ways to check for this specific error 
        // use mysql_error_string, mysql_errno and mysql_field_count
         //
        if(mysql_field_count(&conn) != 0)
        {
            printf("Error occured when fetching the result set\n");
            exit(1);
        }
    }

    // Print the number of rows returned in the set 
    num_rows = mysql_num_rows(res);
    printf ("Number of rows %u\n", (unsigned long)num_rows);// Refer to mysql docs 
                                                         // on the need for this
                                                         // typecasting
                                                          //
    
    // Print the rows
    printf("Rows returned from query \"%s\" are\n", query);
    // Number of fields can change depends on the query
    num_fields = mysql_num_fields(res);
    while((row = mysql_fetch_row(res)) != NULL)
    {   
        int i = 0;
        unsigned long *lengths = 0; // Array of lengths for the current row */
        lengths = mysql_fetch_lengths(res); // Lengths of the fields for the
                                                // current row
                                                //
        // Print each field using mysql_num_fields..this is same for all rows
         // of the table
         //
        for(i = 0 ; i < num_fields; i++)
        {
            // Store these values into strings and then covert to uint64 as 
            // needed
            printf("[%.*s]", (int)lengths[i], row[i]?row[i]:"NULL");
        }
    }

    // Execture an insert query 
    
    // Close connection 
    mysql_free_result(res);
    mysql_close(&conn);
    
    return 0;
}
