import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
from youtube_search import YoutubeSearch
import yt_dlp as youtube_dl
import sys

"""

устанавливаем зависимости
sudo apt-get install ffmpeg
pip3 install yt-dlp
pip3 install spotipy --upgrade
pip3 install youtube-search

получаем ключи для приложения
https://developer.spotify.com/dashboard

потом прописываем их в env
SPOTIPY_CLIENT_ID
SPOTIPY_CLIENT_SECRET

python3 spoty_steal.py <playlist_id>

"""

if len(sys.argv) < 2:
    print("[ERROR] - enter playlist id after script name ( python3 script.py <playlist_id> )")
    exit(0)

playlist_id = sys.argv[1]

spotify = spotipy.Spotify(client_credentials_manager=SpotifyClientCredentials())

def check_duration(range_max, general_min, general_sec, yt):
    for i in range(range_max):
        if yt == f"{general_min}:{general_sec+i:02d}" or yt == f"{general_min}:{general_sec-i:02d}":
            return True
        
    return False

def download_audio(yt_url):
    ydl_opts = {
        'quiet': True,
        'format': 'bestaudio/best',
        'postprocessors': [{
            'key': 'FFmpegExtractAudio',
            'preferredcodec': 'mp3',
            'preferredquality': '320',
        }],
    }
    with youtube_dl.YoutubeDL(ydl_opts) as ydl:
        ydl.download([yt_url])

counter_tracks = 0
total_tracks = spotify.playlist_items(playlist_id=playlist_id, limit=100, 
    offset=100, fields=('total',))['total']


playlist_tracks_all = list()

yt_urls = list()
list_not_finded = list()


while True:
    playlist_tracks_all.append(spotify.playlist_items(playlist_id=playlist_id, limit=100, 
            offset=counter_tracks, fields=('items',)))

    if total_tracks - (counter_tracks + 100) > 0:
        counter_tracks += 100
    else:
        break



for playlist_tracks in playlist_tracks_all:
    for track in playlist_tracks['items']:
        print("artists: ")
        for artist in track['track']['album']['artists']:
            print("\t" + artist['name'])
        
        track_name = track['track']['name']
        track_duration = track['track']['duration_ms']
        track_first_artist_name = track['track']['album']['artists'][0]['name']
        print("track name:\n\t" + track_name)


        minutes = int(track_duration/(1000*60)%60)
        seconds = int(track_duration/1000)%60

        print(f"duration:\n\t{minutes}:{seconds:02d}\n")
        print("YouTube coincidences:")

        finded = False

        for res in YoutubeSearch(track_first_artist_name + " " + track_name, max_results=8).to_dict():
            if check_duration(8, minutes, seconds, res['duration']):
                print("track founded!")

                print(f"title: {res['title']}")
                print(f"duration: {res['duration']}")

                print(f"url_suffix: {res['url_suffix']}")
                yt_urls.append({'title': track_name, 'duration': f"{minutes}:{seconds:02d}", 
                    'url': 'https://youtube.com'+res['url_suffix']})

                finded = True
                break

        if finded != True:
            list_not_finded.append({'title': track_name, 'duration': f"{minutes}:{seconds:02d}"})

        print("\n")

downloaded = 0
for track in yt_urls:
    print(f"{track['title']} - {track['duration']}\n{track['url']}\n")
    download_audio(track['url'])

    downloaded += 1


print(f"\ndownloaded - {downloaded}/{total_tracks}")

with open("finded.txt", "w", encoding='utf-8') as file:
    for track in yt_urls:
        file.write(f"{track['title']} - {track['duration']}\n{track['url']}\n\n")

with open("not_finded.txt", "w", encoding='utf-8') as file:
    for track in list_not_finded:
        file.write(f"{track['title']} - {track['duration']}\n\n")
