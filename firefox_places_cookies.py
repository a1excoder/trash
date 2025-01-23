import sqlite3, os

# on future
"""
import platform
platform.system()

Linux: Linux
Mac: Darwin
Windows: Windows

"""

db_places = "places.sqlite"
db_cookies = "cookies.sqlite"

script_dir = os.path.dirname(os.path.realpath(__file__))

profiles_dir = os.environ['USERPROFILE'] + "\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles"
dir_list = os.listdir(profiles_dir)

def write_places_to_file(dir):
	try:
		conn = sqlite3.connect(f"{dir}\\{db_places}")
		file = open(f"{script_dir}\\places.txt", "w", encoding='utf-8')

		cur = conn.cursor()

		cur.execute("SELECT * FROM `moz_bookmarks`")
		rows = cur.fetchall()

		file.write(f"[DIR INFO] {dir}\n\n\n")

		file.write(f"### [moz_bookmarks] ###\n")
		for row in rows:
			file.write(f"id [{row[0]}] | title [{row[5]}]\n")
		print(f"[SUCCESS] ~[moz_bookmarks]~ writed to \"{script_dir}\\places.txt\"")
		file.write("\n\n")

		################

		cur.execute("SELECT * FROM `moz_places`")
		rows = cur.fetchall()

		file.write(f"### [moz_places] history ###\n")
		for row in rows:
			# !! last_visit_date
			file.write(f"id [{row[0]}] | url[{row[1]}] | title [{row[2]}] | description [{row[12]}] | visit_count [{row[4]}] | last_visit_date [{row[8]}]\n")
		print(f"[SUCCESS] ~[moz_places]~ or history writed to \"{script_dir}\\places.txt\"")
		file.write("\n\n")

		################

		cur.execute("SELECT * FROM `moz_annos`")
		rows = cur.fetchall()

		file.write(f"### [moz_annos] history ###\n")
		for row in rows:
			file.write(f"id [{row[0]}] | place_id [{row[1]}] | content [{row[3]}]\n")
		print(f"[SUCCESS] ~[moz_annos]~ writed to \"{script_dir}\\places.txt\"")
		file.write("\n\n")



		print(f"[SUCCESS] ALL places data writed to \"{script_dir}\\places.txt\"")

	except Exception as e:
		print(f"[ERROR] Exception [{e}]")
	finally:
		if conn:
			conn.close()
		file.close()


def write_cookies_to_file(dir):
	try:
		conn = sqlite3.connect(f"{dir}\\{db_cookies}")
		file = open(f"{script_dir}\\cookies.txt", "w", encoding='utf-8')

		cur = conn.cursor()

		cur.execute("SELECT * FROM `moz_cookies`")
		rows = cur.fetchall()

		file.write(f"[DIR INFO] {dir}\n\n\n")

		file.write(f"### [moz_cookies] ###\n")
		for row in rows:
			file.write(f"id [{row[0]}] | originAttibutes [{row[1]}] | name [{row[2]}] | value [{row[3]}]  | host(url) [{row[4]}]"
				f" | path [{row[5]}] | creationTime [{row[8]}] | lastAccessed [{row[7]}]\n")
		print(f"[SUCCESS] ~[moz_cookies]~ writed to \"{script_dir}\\cookies.txt\"")
		file.write("\n\n")



		print(f"[SUCCESS] ALL cookies data writed to \"{script_dir}\\cookies.txt\"")

	except Exception as e:
		print(f"[ERROR] Exception [{e}]")
	finally:
		if conn:
			conn.close()
		file.close()


if __name__ == '__main__':
	for name in dir_list:
		if (name.find("default-release") != -1):
			print(f"[SUCCESS] dir is found \"{profiles_dir}\\{name}\\\"")
			write_places_to_file(profiles_dir + "\\" + name)
			write_cookies_to_file(profiles_dir + "\\" + name)
			# lalala
			exit(0)

	print("[ERROR] dir not found")
