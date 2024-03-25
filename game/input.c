//
// Created by Karim on 3/25/2024.
//

void doKeyDown(SDL_KeyboardEvent *event, game_input_t *input)
{
    if (event->repeat == 0)
    {
        if (event->keysym.scancode == SDL_SCANCODE_UP)
        {
            input->up = 1;
        }

        if (event->keysym.scancode == SDL_SCANCODE_DOWN)
        {
            input->down = 1;
        }

        if (event->keysym.scancode == SDL_SCANCODE_LEFT)
        {
            input->left = 1;
        }

        if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
        {
            input->right = 1;
        }
    }
}

void doKeyUp(SDL_KeyboardEvent *event, game_input_t *input)
{
    if (event->repeat == 0)
    {
        if (event->keysym.scancode == SDL_SCANCODE_UP)
        {
            input->up = 0;
        }

        if (event->keysym.scancode == SDL_SCANCODE_DOWN)
        {
            input->down = 0;
        }

        if (event->keysym.scancode == SDL_SCANCODE_LEFT)
        {
            input->left = 0;
        }

        if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
        {
            input->right = 0;
        }
    }
}
