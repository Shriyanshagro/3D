# D.N.A.H.B GAMES , (a.k.a "E.N.D")
Assignment under Graphics Course

Story of motivation:
There's dnahb_man need to reach the diagonally end point of land from starting point.

Rules and controls:

    To play the game:
        run the file sample2D in terminal , just by typing ./sample2D in terminal.

    To compile the code , run
        sudo g++ -o sample2D Sample_GL3_2D.cpp -lGL -lGLU -lGLEW -lglut -lm -lsfml-audio

    Controls:

        dnahb_man's control:
            a ==> left move
            d ==> right move
            w ==> up move
            s ==> down move
            spacebar ==> jump
            f ==> flash headlight
            c ==> alter the jumping axis.
            v ==> alter the jumping direction on same axis.
            b ==> decreases speed of movement
            n ==> increases speed of movement
            g ==> decreases jumping speed
            h ==> increases jumping speed


        Screen control:
            arrow_key_UP/scroll up ==> zoom in
            arrow_key_DOWN/scroll down ==> zoom out
            arrow_key_LEFT ==> panx increnment
            arrow_key_RIGHT ==> panx decrement

        Camera :
            5 Camera View Available:
                 *  Adventurer View: View from the adventurers head position, where one can see part of the obstacle
                    course in the front.
                 *  Follow-cam View: A view of the person and the region in front of him from a location above and behind the person as if a camera is following him from above.
                 *  Tower View: The camera sitting on a tower to the side of the obstacle course observing it at an angle.
                 *  Top View: A simple view from directly above, looking down. While this view is good to see the whole course, it does not reveal rising and lowering tiles very clearly.
                 * Helicopter Cam: Here the camera is movable with the mouse in an intuitive manner. Click and drag should change the look angle, up vector is always up, and scroll wheel will move the camera close or away from the scene.

            Controls:
                Mouse Leftclick\Keyboard's enter key = change camera view


    Some twists in game:

        * You only have single life, to complete the game .
        * there's a trappy holes(black cloured) , if anyhow your dnahb_man ever try to cross from over it, you gotta lose you life in a jiff!!
        * Level's have been impelemented , so each time you reach the destination , the dnahb game going to be nasty !!
        * Also helicopter's sound have been implemented while you are using helicopter view.
