#! /usr/bin/env python3

import random as r
import threading as th
import tkinter as tk

# Gampelay constants
ROCK     = 0
SCISSORS = 1
PAPER    = 2

# Score variables
PLAYER_SCORE = 0
EYEGUY_SCORE = 0

# Threading timer object
TIMER = None

def main():
    """ Main method of the game. """

    def reset():
        """ Restarts the game. """
        global PLAYER_SCORE, EYEGUY_SCORE
        PLAYER_SCORE = 0
        EYEGUY_SCORE = 0
        player_label["text"] = "Humanoid>   " + str(PLAYER_SCORE)
        eyeguy_label["text"] = str(EYEGUY_SCORE) + "   <EyeGuy"
        alien_at_ease()

    def alien_at_ease():
        """ Resets the EyeGuy label to it's base resting state. """
        global TIMER
        eyeguy.config(image = alien_graphics["resting"])
        print("Eyeguy is resting.")
        TIMER = None

    def quit_game():
        """ Cancels any pending timer ad closes the window. """
        if TIMER is not None:
            TIMER.cancel()
        window.quit()

    def play(player_choice):
        """ Evaluates a game turn. """
        global PLAYER_SCORE, EYEGUY_SCORE, TIMER

        def choice2str(choice):
            """ Returns the name of a gameplay move as a string. """
            return "ROCK" if choice == ROCK else \
                ("SCISSORS" if choice == SCISSORS else "PAPER")

        # Get EyeGuy's move
        alien_choice = r.randrange(3)

        # Check the outcome of this turn
        if (alien_choice == ROCK and player_choice == SCISSORS) or \
           (alien_choice == SCISSORS and player_choice == PAPER) or \
           (alien_choice == PAPER and player_choice == ROCK):
            # Check if EyeGuy won
            print("Eyeguy won! " + choice2str(alien_choice) + " beats " + choice2str(player_choice))
            EYEGUY_SCORE += 1

            # Update EyeGuy's sprite
            if alien_choice == ROCK:
                eyeguy.config(image = alien_graphics["rock_win"])
            elif alien_choice == SCISSORS:
                eyeguy.config(image = alien_graphics["scsr_win"])
            else:
                eyeguy.config(image = alien_graphics["papr_win"])

        elif (alien_choice == ROCK and player_choice == PAPER) or \
             (alien_choice == SCISSORS and player_choice == ROCK) or \
             (alien_choice == PAPER and player_choice == SCISSORS):
            # Check if EyeGuy lost
            print("Player won! " + choice2str(player_choice) + " beats " + choice2str(alien_choice))
            PLAYER_SCORE += 1

            # Update EyeGuy's sprite
            if alien_choice == ROCK:
                eyeguy.config(image = alien_graphics["rock_dft"])
            elif alien_choice == SCISSORS:
                eyeguy.config(image = alien_graphics["scsr_dft"])
            else:
                eyeguy.config(image = alien_graphics["papr_dft"])

        else:
            # Else its a tie
            print(choice2str(alien_choice) + " and " + choice2str(player_choice) + " is a tie!")

            # Update EyeGuy's sprite
            if alien_choice == ROCK:
                eyeguy.config(image = alien_graphics["rock_tie"])
            elif alien_choice == SCISSORS:
                eyeguy.config(image = alien_graphics["scsr_tie"])
            else:
                eyeguy.config(image = alien_graphics["papr_tie"])

        # Update the score labels
        player_label["text"] = "Humanoid>   " + str(PLAYER_SCORE)
        eyeguy_label["text"] = str(EYEGUY_SCORE) + "   <EyeGuy"

        # Set a timer to reset EyeGuy's sprite after 3 seconds
        if TIMER is not None:
            print("Canceled!")
            TIMER.cancel()
        TIMER = th.Timer(3, alien_at_ease)
        TIMER.start()

    # Create the game window
    window = tk.Tk()
    window.wm_title("Rock and Scissors VS Paper")
    window.wm_resizable(width = tk.FALSE, height = tk.FALSE)

    # Create the main menu bar
    menubar = tk.Menu(window)

    # Create the "game" menu option
    game_menu = tk.Menu(menubar, tearoff = 0)
    game_menu.add_command(label = "New Game", command = reset)
    game_menu.add_command(label = "Quit", command = quit_game)

    # Add the menu options to the menu bar
    menubar.add_cascade(label = "Game", menu = game_menu)
    window.config(menu = menubar)

    # Load EyeGuy's sprites
    alien_graphics = {
        "resting" : tk.PhotoImage(file = "assets/gfx/eyeguy.1.1.png"),
        "rock_tie" : tk.PhotoImage(file = "assets/gfx/eyeguy.2.1.png"),
        "rock_win" : tk.PhotoImage(file = "assets/gfx/eyeguy.2.2.png"),
        "rock_dft" : tk.PhotoImage(file = "assets/gfx/eyeguy.2.3.png"),
        "scsr_tie" : tk.PhotoImage(file = "assets/gfx/eyeguy.3.1.png"),
        "scsr_win" : tk.PhotoImage(file = "assets/gfx/eyeguy.3.2.png"),
        "scsr_dft" : tk.PhotoImage(file = "assets/gfx/eyeguy.3.3.png"),
        "papr_tie" : tk.PhotoImage(file = "assets/gfx/eyeguy.4.1.png"),
        "papr_win" : tk.PhotoImage(file = "assets/gfx/eyeguy.4.2.png"),
        "papr_dft" : tk.PhotoImage(file = "assets/gfx/eyeguy.4.3.png")
    }

    # Create and set the score labels
    player_label = tk.Label(window, text = "Humanoid>   " + str(PLAYER_SCORE), fg = "red")
    player_label.grid(row = 0, column = 0)
    eyeguy_label = tk.Label(window, text = str(EYEGUY_SCORE) + "   <EyeGuy", fg = "blue")
    eyeguy_label.grid(row = 0, column = 2)

    # Create and set EyeGuy's sprite label
    eyeguy = tk.Label(window, image = alien_graphics["resting"])
    eyeguy.grid(row = 1, column = 0, columnspan = 3)

    # Load the icons for the buttons
    btn_icons = [tk.PhotoImage(file = "assets/gfx/btn.1.png"),
                 tk.PhotoImage(file = "assets/gfx/btn.2.png"),
                 tk.PhotoImage(file = "assets/gfx/btn.3.png")]

    # Create and set each button
    rock_btn = tk.Button(window, image = btn_icons[0], relief = tk.FLAT, command = lambda: play(0))
    rock_btn.grid(row = 2, column = 0)
    scsr_btn = tk.Button(window, image = btn_icons[1], relief = tk.FLAT, command = lambda: play(1))
    scsr_btn.grid(row = 2, column = 1)
    papr_btn = tk.Button(window, image = btn_icons[2], relief = tk.FLAT, command = lambda: play(2))
    papr_btn.grid(row = 2, column = 2)

    # Create and set the author label
    text = tk.Label(window, text = "http://github.com/miky-kr5/")
    text.grid(row = 3, column = 0, columnspan = 3)

    # Show the window
    try:
        window.mainloop()
    except KeyboardInterrupt:
        print("Interrupted...")

# Game entry point
if __name__ == "__main__":
    main()
