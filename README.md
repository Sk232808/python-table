# Student Marks Table

A small Flask web application that displays student marks in a readable table. It also calculates and shows the total marks and average marks.

## Live Demo

View the application online:

https://glittering-pudding-320220.netlify.app/

## What This Project Shows

- A simple Python Flask application
- A student marks table
- Total marks calculation
- Average marks calculation
- A responsive web page that runs in a browser

## Project Files

- `shvam.py` - Flask application and web page
- `requirements.txt` - Python package required by the application

## Run It On Your Computer

### 1. Open the project folder

```powershell
cd C:\Users\shiva\dummy
```

### 2. Install Flask

```powershell
py -m pip install -r requirements.txt
```

### 3. Start the application

```powershell
py shvam.py
```

### 4. Open the website

Open this address in your browser:

```text
http://127.0.0.1:5000
```

Press `Ctrl+C` in the terminal to stop the application.

## Publish Changes To GitHub

The project repository is:

https://github.com/Sk232808/python-table

After changing the code, run these commands from the project folder:

```powershell
git add .
git commit -m "Describe your changes"
git push
```

Refresh the GitHub page to see the updated files.

## Deploy The Flask App

This project is a Flask server, so it cannot be deployed directly to Netlify as a normal static site. Netlify is designed mainly for static HTML, CSS, and JavaScript files.

For the current Flask version, use a Python-compatible host such as Render:

1. Create an account at https://render.com.
2. Choose **New** and then **Web Service**.
3. Connect the GitHub repository `Sk232808/python-table`.
4. Set **Build Command** to:

   ```text
   pip install -r requirements.txt
   ```

5. Set **Start Command** to:

   ```text
   gunicorn shvam:app
   ```

6. Choose the free plan if it is available, then click **Create Web Service**.

The live website URL will be provided by Render after deployment.
