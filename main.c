/**		                MAIN.C
 * This is an x window client disguised as a de or wm.
 * 
 * Useful stuff
 *      xcb's source code - https://gitlab.freedesktop.org/xorg/lib/libxcb 
 *      xcb structs - https://xcb.freedesktop.org/manual/annotated.html
 *      implementation wm via xcb (i3wm) - https://github.com/i3/i3
 *      i3 docs - https://i3wm.org/docs/hacking-howto.html
 *      gtk3 - https://docs.gtk.org/gtk3/
 *      glib-2.0 - https://docs.gtk.org/glib/
 */

// get gtk working and filling up the entire xcb window

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <gtk/gtk.h>
//#include <glib.h>




static void
activate (GtkApplication* app,
          gpointer        user_data)
{
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_widget_show_all (window);
}

int main(int argc, char** argv)
{
    /*
    xcb_point_t polyline[] = {
        {50, 10},
        { 5, 20},     // rest of points are relative 
        {25,-20},
        {10, 10}
    };
    */

    /* Open the connection to the X server */
    int connection_screen;
    xcb_connection_t *connection = xcb_connect (NULL, &connection_screen);
    if (xcb_connection_has_error(connection)) {
        printf("xcb_connect returned an error");
        exit(0);
    }

    /* Get the first screen */
    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    // define rectangles after you pull screen stats with xcb_setup_roots_iterator
    xcb_rectangle_t rectangles[] = {
        {0, 0, (--screen->width_in_pixels), (--screen->height_in_pixels)}
    };

    /* Create [color] (foreground) graphic context */
    xcb_drawable_t  window     = screen->root;
    xcb_gcontext_t  foreground = xcb_generate_id (connection);
    uint32_t        mask       = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t        values[2]  = {screen->white_pixel, 0};

    xcb_create_gc (connection, foreground, window, mask, values);

    printf ("\n%i\t%i\n", screen->width_in_pixels, screen->height_in_pixels);


    /* Create a window */
    window = xcb_generate_id (connection);

    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->black_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window ( 
                        connection,                    /* connection     */
                        XCB_COPY_FROM_PARENT,          /* depth          */
                        window,                        /* window Id      */
                        screen->root,                  /* parent window  */
                        0,                             /* x              */ 
                        0,                             /* y              */
                        screen->width_in_pixels,       /* width          */
                        screen->height_in_pixels,      /* height         */
                        0,                             /* border_width   */
                        XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class          */
                        screen->root_visual,           /* visual         */
                        mask, values                   /* masks          */
                      );


    /* Map the window on the screen and flush*/
    xcb_map_window (connection, window);
    xcb_flush (connection);


    /* Create gtk instance */
    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);



    // draw primitives 
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event (connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_EXPOSE:
                // draw the rectangles 
                //xcb_poly_rectangle (connection, window, foreground, 1, rectangles);
 
                // draw the polygonal line 
                //xcb_poly_line (connection, XCB_COORD_MODE_PREVIOUS, window, foreground, 4, polyline);

                // flush the request 
                xcb_flush (connection);

                break;
            default: 
                // Unknown event type, ignore it 
                break;
        }

        free (event);
    }

    return 0;
}