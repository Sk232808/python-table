from flask import Flask, render_template_string

app = Flask(__name__)

marks = [
    {"subject": "CPC", "marks": 88},
    {"subject": "Python", "marks": 95},
    {"subject": "OS", "marks": 82},
    {"subject": "DBMS", "marks": 90},
]

@app.route("/")
def index():
    total = sum(student["marks"] for student in marks)
    average = total / len(marks) if marks else 0

    return render_template_string(
        """
        <!doctype html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Student Marks Table</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background: #f4f7fb;
                    margin: 0;
                    padding: 40px;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    min-height: 100vh;
                }
                .container {
                    background: white;
                    padding: 30px;
                    border-radius: 12px;
                    box-shadow: 0 6px 20px rgba(0,0,0,0.08);
                    width: 600px;
                }
                h1 {
                    text-align: center;
                    color: #1f2937;
                    margin-bottom: 20px;
                }
                table {
                    width: 100%;
                    border-collapse: collapse;
                    margin-top: 10px;
                }
                th, td {
                    border: 1px solid #d1d5db;
                    padding: 12px 16px;
                    text-align: center;
                }
                th {
                    background: #2563eb;
                    color: white;
                }
                tr:nth-child(even) {
                    background: #f9fafb;
                }
                .summary {
                    margin-top: 20px;
                    font-weight: bold;
                    color: #111827;
                    text-align: center;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>Student Marks</h1>
                <table>
                    <thead>
                        <tr>
                            <th>Subject</th>
                            <th>Marks</th>
                        </tr>
                    </thead>
                    <tbody>
                        {% for item in marks %}
                        <tr>
                            <td>{{ item.subject }}</td>
                            <td>{{ item.marks }}</td>
                        </tr>
                        {% endfor %}
                    </tbody>
                </table>
                <div class="summary">
                    Total Marks: {{ total }} &nbsp; | &nbsp; Average: {{ '%.2f' % average }}
                </div>
            </div>
        </body>
        </html>
        """,
        marks=marks,
        total=total,
        average=average,
    )


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
