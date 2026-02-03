import os
import psycopg2
import pytest

POSTGRES_PASSWORD = os.environ.get("POSTGRES_PASSWORD", "test")
POSTGRES_USER = os.environ.get("POSTGRES_USER", "test")
POSTGRES_DB = os.environ.get("POSTGRES_DB", "test")
DB_PORT = os.environ.get("DB_PORT", 5432)
DB_HOST = "db"
APP_HOST = "app"
APP_PORT = os.environ.get("APP_PORT", 8000)

conn = psycopg2.connect(
    dbname=POSTGRES_DB,
    user=POSTGRES_USER,
    password=POSTGRES_PASSWORD,
    host=DB_HOST,
    port=DB_PORT,
)


def pytest_configure(config):
    """Регистрация кастомных меток для pytest"""
    config.addinivalue_line(
        "markers", "endpoint(name): маркировка тестов по эндпоинтам"
    )


def clear_database():
    with conn.cursor() as cur:
        cur.execute(
            """
            DO $$
            DECLARE
                r RECORD;
            BEGIN
                FOR r IN (
                    SELECT tablename
                    FROM pg_tables
                    WHERE schemaname = 'public'
                )
                LOOP
                    EXECUTE 'TRUNCATE TABLE '
                        || quote_ident(r.tablename)
                        || ' RESTART IDENTITY CASCADE';
                END LOOP;
            END $$;
        """
        )
    conn.commit()


@pytest.fixture(autouse=True)
def clean_db():
    clear_database()
    yield
