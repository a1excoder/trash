# pip3 install pyTelegramBotAPI
# 1. set a bot token to main.py file
# 2. start a script
# 3. send /start command to bot and get your chat id
# 4. stop bot and write in concole: python3 main.py <chat_bot_id> <path_to_file>

import telebot
import sys
import os

# 50 Megabytes = 52,428,800 Bytes
MAX_FILE_SIZE = 50000000

API_KEY = "<TOKEN>"

bot = telebot.TeleBot(API_KEY)

@bot.message_handler(commands=['start'])
def get_chat_id(message):
    bot.send_message(message.chat.id, f"chat id: \"{message.chat.id}\"")
    print(f"username: \"@{message.chat.username}\" | chat id: \"{message.chat.id}\"")

def send_file(file, chat_id):
    file = open(sys.argv[2], "rb")
    bot.send_document(chat_id, file)
    file.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        bot.polling(none_stop=False)
    else:
        if os.path.exists(sys.argv[2]) == True:
            if os.path.getsize(sys.argv[2]) <= MAX_FILE_SIZE:
                send_file(sys.argv[2], sys.argv[1])
            else:
                print(f"max file size is {MAX_FILE_SIZE} bytes")
        else:
            print(f"file \"{sys.argv[2]}\" not exists")
