#!/usr/bin/env python

from subprocess import check_output
import re
import json
import os.path
from licenses_vivaldi_texts import onlineLicenses

basepath = "../../vivapp"
maindeps = check_output(["python", basepath+"/bin/list_jsdeps.py", basepath, "src/app.js"])

modules = {}

for m in re.findall(r"(.*node_modules/([^/]+))", maindeps):
  moduledir = m[0]
  modulename = m[1]
  if (moduledir in modules
     or (modulename == "chrome") # ours
     or (modulename == "wo-stringencoding") # ours
     or modulename == "vivaldi-color" # ours
     or (modulename == "url") # ignore for now, LICENSE file is on master, https://github.com/defunctzombie/node-url
     or (modulename == "ripemd160") # ignore for now, LICENSE file is on master, https://github.com/cryptocoinjs/ripemd160
     or (modulename == "indexof") # trivial
     or (modulename == "binary-search") # CC0-1.0, no need to put in credits file
    ):
      continue
  entry = {"name": modulename}

  # get license file (in order of preference)
  for l in ["LICENSE-MIT", "LICENSE-MIT.TXT", "LICENSE.MIT", "LICENSE.BSD",
      "LICENSE.APACHE2", "LICENSE", "LICENSE.txt", "LICENSE.md", "License",
      "license.txt", "License.md", "LICENSE.mkd"]:
    if os.path.exists(basepath+"/"+moduledir+"/"+l):
      f = open(basepath+"/"+moduledir+"/"+l)
      entry["license"] = f.read()
      f.close()
      break

  # get one word license type from package.json
  f = open(basepath+"/"+moduledir+"/package.json")
  pjson = json.loads(f.read())
  f.close()
  if "license" in pjson:
    entry["licensetype"] = pjson["license"]
  elif "licence" in pjson: # typo in react-list
    entry["licensetype"] = pjson["licence"]
  elif "licenses" in pjson:
    if type(pjson["licenses"]) is str:
      entry["licensetype"] = pjson["licenses"]
    elif type(pjson["licenses"]) is list:
      entry["licensetype"] = pjson["licenses"][0]["type"]
      entry["licenseurl"] = pjson["licenses"][0]["url"]
    elif type(pjson["licenses"]) is dict:
      entry["licensetype"] = pjson["licenses"]["type"]
      entry["licenseurl"] = pjson["licenses"]["url"]
  if "licensetype" in entry and entry["licensetype"] not in ["MIT", "BSD",
      "Apache 2.0", "Apache-2.0", "Apache License, Version 2.0", "Creative Commons Attribution 2.5 License", "MPL",
      "BSD-3-Clause", "ISC"]:
    print("ERROR: " + moduledir + " uses a license that hasn't been reviewed for Vivaldi: " + entry["licensetype"])
    exit(1)

  if not "license" in entry and "licenseurl" in entry:
    if entry["licenseurl"] in onlineLicenses:
      entry["license"] = onlineLicenses[entry["licenseurl"]]
    else:
      print("ERROR: " + modulename + " provides URL " + entry["licenseurl"] + " as a license but it hasn't been copied to licenses_vivaldi_texts.py")
      exit(1)
  if not "license" in entry and "licensetype" in entry:
    entry["license"] = entry["licensetype"]

  if not "license" in entry:
    print("ERROR: License statement missing for module " + moduledir + ". Add it to the list of exceptions in licenses_vivaldi.py if it's not a third party module.")
    exit(1)

  if "homepage" in pjson:
    entry["url"] = pjson["homepage"]
  else:
    entry["url"] = ("https://www.npmjs.org/package/"+entry["name"])

  if "licensetype" in entry and entry["licensetype"] == "MPL":
    entry["license_unescaped"] = "Source code is available for download at <a href='http://registry.npmjs.org/"+modulename+"/-/"+modulename+"-"+pjson["version"]+".tgz'>http://registry.npmjs.org/"+modulename+"/-/"+modulename+"-"+pjson["version"]+".tgz<a/>. No source code files were modified for use in Vivaldi."
  else:
    entry["license_unescaped"] = ""

  for e in entry:
    try:
      entry[e] = entry[e].encode("ASCII")
    except:
      pass
  modules[moduledir] = entry
