#include <gtk/gtk.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

int clientSocket;
GtkWidget *textView;
GtkTextBuffer *textBuffer;

void AddMessage(const std::string& message)
{
    GtkTextIter endIter;
    gtk_text_buffer_get_end_iter(textBuffer, &endIter);
    gtk_text_buffer_insert(textBuffer, &endIter, (message + "\n").c_str(), -1);
}

void SendMessageToServer(const std::string& message)
{
    send(clientSocket, message.c_str(), message.size(), 0);
}

void ReceiveMessages()
{
    char buffer[256];
    int result;
    while (true) {
        result = recv(clientSocket, buffer, 256, 0);
        if (result > 0) {
            buffer[result] = '\0';
            AddMessage("Server: " + std::string(buffer));
        } else if (result == 0) {
            break;
        } else {
            std::cerr << "Receive failed" << std::endl;
            break;
        }
    }
}

void on_send_button_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *inputEntry = GTK_WIDGET(user_data);
    const char *message = gtk_entry_get_text(GTK_ENTRY(inputEntry));
    if (strlen(message) > 0) {
        AddMessage("Me: " + std::string(message));
        SendMessageToServer(message);
        gtk_entry_set_text(GTK_ENTRY(inputEntry), "");
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the text view for displaying messages
    textView = gtk_text_view_new();
    textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textView), FALSE);
    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), textView);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 0);

    // Create the entry box for input
    GtkWidget *entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    // Create the send button
    GtkWidget *sendButton = gtk_button_new_with_label("Send");
    gtk_box_pack_start(GTK_BOX(vbox), sendButton, FALSE, FALSE, 0);

    g_signal_connect(sendButton, "clicked", G_CALLBACK(on_send_button_clicked), entry);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Initialize socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP
    server.sin_port = htons(1234); // Server port

    if (connect(clientSocket, (struct sockaddr*)&server, sizeof(server)) == -1) {
        std::cerr << "Connection to server failed" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Start receiving messages in a separate thread
    std::thread recvThread(ReceiveMessages);
    recvThread.detach();

    gtk_widget_show_all(window);
    gtk_main();

    close(clientSocket);

    return 0;
}
