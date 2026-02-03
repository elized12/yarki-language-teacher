# .docker/test/Dockerfile
FROM python:3.11-slim

WORKDIR /test

COPY ./tests ./tests

RUN pip install --no-cache-dir \
    pytest \
    requests \
    python-dotenv \
    psycopg2-binary \
    pytest-html

CMD ["python", "-m", "pytest", "tests/integration/", "-v", "--html=test-reports/report.html", "--self-contained-html"]