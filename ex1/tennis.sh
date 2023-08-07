#!/bin/bash
play=1
#the points of player 1
points_1=50
#points of player 2
points_2=50
#the number that player 1 pick
number_1=0
#the number that player 2 pick
number_2=0
#the place of the ball in the board
place_of_ball=0
flag=1
win=0
#win of game- 1,2 or 3 for tie
win_of_game=0
print_initial_board () {
  echo " Player 1: $points_1         Player 2: $points_2 "
  echo " --------------------------------- "
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
  echo " |       |       O       |       | "
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
  echo " --------------------------------- "
}

print_the_ball(){
case "$place_of_ball" in
    -3)
        echo "O|       |       #       |       | "
        ;;
    -2)
        echo " |   O   |       #       |       | "
        ;;
    -1)
        echo " |       |   O   #       |       | "
        ;;
    0)
        echo " |       |       O       |       | "
        ;;
    1)
        echo " |       |       #   O   |       | "
        ;;
    2)
        echo " |       |       #       |   O   | "
        ;;
    3)
        echo " |       |       #       |       |O"
        ;;
esac
}
print_board_after_change() {
  echo " Player 1: $points_1         Player 2: $points_2 "
  echo " --------------------------------- "
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
  print_the_ball
  echo " |       |       #       |       | "
  echo " |       |       #       |       | "
  echo " --------------------------------- "
  echo -e "       Player 1 played: $number_1\n       Player 2 played: $number_2\n\n"
}

change_points(){
points_1=$[$points_1-number_1]
points_2=$[$points_2-number_2]
}


get_number_1(){
    while (($flag)) 
    do
        echo "PLAYER 1 PICK A NUMBER: "
        read -s number_1
        if [[ $number_1 =~ ^[0-9]+$ && $number_1 -le $points_1 ]]
        then
           flag=0
        else
            echo "NOT A VALID MOVE !"
        fi
    done
flag=1
}

get_number_2(){
    # function to get a valid move for player 2
    while (($flag)) 
    do
        echo "PLAYER 2 PICK A NUMBER: "
        # read input from player 2 without echoing it
        read -s number_2 
        # if the input is a positive integer and less than or equal to the points of player 2
        if [[ $number_2 =~ ^[0-9]+$ && $number_2 -le $points_2 ]] 
        then
        # set flag to 0 to break out of the while loop
           flag=0 
        else
        # otherwise print an error message and continue the loop
            echo "NOT A VALID MOVE !"
        fi
    done
    # reset flag for the next round
    flag=1 
}

winner_of_round(){
    # if the number picked by player 1 is less than the number picked by player 2
    if [ $number_1 -lt $number_2 ] 
    then
    # set win to 2 to indicate that player 2 has won the round
        win=2 
    # if the number picked by player 2 is less than the number picked by player 1
    elif [ $number_2 -lt $number_1 ] 
    then
    # set win to 1 to indicate that player 1 has won the round
        win=1 
    else
    # set win to 3 to indicate that it's a draw
        win=3 
    fi
}

replace_ball(){
    
    case "$win" in
    1) # if player 1 wins
        #the ball in the side of looser
        if [[ $place_of_ball -eq 1 || $place_of_ball -eq 2 ]] 
        then
        # move the ball one step towards the winner
            place_of_ball=$[$place_of_ball+1] 
        #the ball in the side of winner
        else 
        # move the ball to the starting point of the winner
            place_of_ball=1 
        fi
        # update the points of the players
        change_points 
    ;;
    2) # if player 2 wins
    # if the ball is on the side of the loser
        if [[ $place_of_ball -eq -1 || $place_of_ball -eq -2 ]] 
        then
        # move the ball one step towards the winner
            place_of_ball=$[$place_of_ball-1] 
        #the ball in the side of winner
        else 
        # move the ball to the starting point of the winner
            place_of_ball=-1 
        fi
        # update the points of the players
        change_points 
    ;;
    3) # if it's a draw
    # update the points of the players without changing the position of the ball
        change_points 
    ;;
    esac
}

# This function checks who wins the game based on the current game state
check_who_win(){
    # If the ball is in the center position, player 1 wins
    if [ $place_of_ball -eq 3 ]
    then
        win_of_game=1
    # If the ball is in the opposite center position, player 2 wins
    elif [ $place_of_ball -eq -3 ]
    then
        win_of_game=2
    # If player 1 has 0 points and player 2 has more than 0 points, player 2 wins
    elif [[ $points_1 -eq 0 && $points_2 -gt 0 ]]
    then
        win_of_game=2
    # If player 2 has 0 points and player 1 has more than 0 points, player 1 wins
    elif [[ $points_2 -eq 0 && $points_1 -gt 0 ]]
    then 
        win_of_game=1
    # If both players have 0 points and the ball is not in the center position, the player with the ball wins
    elif [[ $points_1 -eq 0 && $points_2 -eq 0 ]]
    then
        if [ $place_of_ball -eq 0 ]
        then 
            win_of_game=3
        elif [[ $place_of_ball -eq 1 || $place_of_ball -eq 2 || $place_of_ball -eq 3 ]]
        then
            win_of_game=1
        else
            win_of_game=2
        fi
    # Otherwise, the game continues
    else
        play=1
    fi
}

# This function prints the winner of the game based on the result from the check_who_win function
print_winner(){
    case "$win_of_game" in
    1)
        echo "PLAYER 1 WINS !" 
        ;;
    2)
        echo "PLAYER 2 WINS !"
        ;;
    3)
        echo "IT'S A DRAW !"
        ;;
    *)
    ;;
    esac
}

# This function runs the game by calling all the necessary functions
game(){
    print_initial_board
    # The game continues as long as play=1
    while(($play))
    do
        get_number_1
        get_number_2
        winner_of_round
        replace_ball
        print_board_after_change
        play=0
        check_who_win
        print_winner
    done
}

# This line calls the game function to start the game
game

