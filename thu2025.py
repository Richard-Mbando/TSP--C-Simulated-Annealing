# talent_hub.py: Unified TalentHub Prototype
# Consolidated for discovery: A SpaceX-caliber talent platform in one file.
# Run with: uvicorn talent_hub:app --reload
# Probe: What happens if we spike traffic on /search? How might we trace it?

import asyncio
import json
import logging
from datetime import datetime, timedelta, date
from enum import Enum
from typing import Optional, List, Dict, Any
from uuid import UUID, uuid4
import uuid as uuid_module

# External mocks/stubs (in a real build, pip install fastapi sqlalchemy asyncpg etc.)
from pydantic import BaseModel, Field, EmailStr, validator
from pydantic_settings import BaseSettings
from fastapi import FastAPI, Depends, HTTPException, status, UploadFile, File, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.trustedhost import TrustedHostMiddleware
from fastapi.responses import JSONResponse
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials, OAuth2PasswordRequestForm
from sqlalchemy import Column, String, DateTime, Boolean, Integer, Text, ForeignKey, Table, Numeric, Date, Enum as SQLEnum, select, func, and_
from sqlalchemy.dialects.postgresql import UUID as PG_UUID
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.ext.asyncio import AsyncSession, create_async_engine, async_sessionmaker
from sqlalchemy.orm import relationship, selectinload
import bcrypt
import jwt
from contextlib import asynccontextmanager
import uvicorn
from logging.config import dictConfig

# Mock external libs (for prototype; replace with real imports)
class MockRedis:
    def __init__(self): self.data = {}
    async def hset(self, key, mapping=None, *args): self.data[key] = mapping or args
    async def zadd(self, key, mapping): pass
    async def smembers(self, key): return set()
    async def sadd(self, key, *members): pass
    async def sismember(self, key, member): return False
    async def zrevrange(self, key, start, end): return []
    async def hgetall(self, key): return self.data.get(key, {})
    async def delete(self, key): self.data.pop(key, None)

class MockElasticsearch:
    async def search(self, **kwargs): return {"hits": {"hits": [], "total": {"value": 0}}}
    async def index(self, **kwargs): pass
    async def delete(self, **kwargs): pass

class MockOpenAI:
    async def chat(self, **kwargs): return type('Obj', (), {'choices': [type('Choice', (), {'message': type('Msg', (), {'content': 'Mock response'})})]})()
    async def embeddings(self, **kwargs): return type('Obj', (), {'data': [type('Data', (), {'embedding': [0.1] * 1536})]})()

class MockS3:
    def put_object(self, **kwargs): return {'Location': 'mock_url'}

class MockPinecone:
    def __init__(self): self.index = MockIndex()
class MockIndex:
    def query(self, **kwargs): return {'matches': []}
    def upsert(self, **kwargs): pass

# Settings
class Settings(BaseSettings):
    DATABASE_URL: str = "sqlite+aiosqlite:///./test.db"  # Async SQLite for prototype
    JWT_SECRET: str = "test-secret"
    ENCRYPTION_KEY: str = "test-key"
    REDIS_HOST: str = "localhost"
    REDIS_PORT: int = 6379
    ELASTICSEARCH_URL: str = "http://localhost:9200"
    OPENAI_API_KEY: str = "test"
    PINECONE_API_KEY: str = "test"
    CORS_ORIGINS: List[str] = ["*"]
    ALLOWED_HOSTS: List[str] = ["*"]
    ENVIRONMENT: str = "development"
    class Config: env_file = ".env"

settings = Settings()

# Logging
dictConfig({
    "version": 1, "disable_existing_loggers": False,
    "formatters": {"default": {"format": "%(asctime)s - %(name)s - %(levelname)s - %(message)s"}},
    "handlers": {"console": {"class": "logging.StreamHandler", "level": "INFO", "formatter": "default"}},
    "root": {"level": "INFO", "handlers": ["console"]},
})
logger = logging.getLogger(__name__)

# Database
Base = declarative_base()
engine = create_async_engine(settings.DATABASE_URL, echo=True if settings.ENVIRONMENT == "development" else False)
AsyncSessionLocal = async_sessionmaker(engine, class_=AsyncSession, expire_on_commit=False)

async def get_db() -> AsyncSession:
    async with AsyncSessionLocal() as session:
        try:
            yield session
        finally:
            await session.close()

# Enums
class UserRole(str, Enum):
    JOB_SEEKER = "job_seeker"
    EMPLOYER = "employer"
    ADMIN = "admin"
    RECRUITER = "recruiter"
    HIRING_MANAGER = "hiring_manager"

class SkillLevel(str, Enum):
    BEGINNER = "beginner"
    INTERMEDIATE = "intermediate"
    ADVANCED = "advanced"
    EXPERT = "expert"

class ProfileVisibility(str, Enum):
    PUBLIC = "public"
    PRIVATE = "private"
    EMPLOYERS_ONLY = "employers_only"

# Models
class User(Base):
    __tablename__ = "users"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    email = Column(String(255), unique=True, index=True, nullable=False)
    hashed_password = Column(String(255), nullable=False)
    role = Column(SQLEnum(UserRole), nullable=False)
    organization_id = Column(PG_UUID(as_uuid=True), nullable=True)
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime, default=datetime.utcnow)
    last_login_at = Column(DateTime, nullable=True)
    profile = relationship("TalentProfile", back_populates="user", uselist=False)

class TalentProfile(Base):
    __tablename__ = "talent_profiles"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    user_id = Column(PG_UUID(as_uuid=True), ForeignKey('users.id'), unique=True)
    first_name = Column(String(100), nullable=False)
    last_name = Column(String(100), nullable=False)
    email = Column(String(255), nullable=False)
    phone = Column(String(20), nullable=True)
    summary = Column(Text, nullable=True)
    resume_url = Column(String(500), nullable=True)
    visibility = Column(SQLEnum(ProfileVisibility), default=ProfileVisibility.PUBLIC)
    completeness_score = Column(Numeric(5, 2), default=0.0)
    created_at = Column(DateTime, default=datetime.utcnow)
    updated_at = Column(DateTime, default=datetime.utcnow, onupdate=func.now())
    user = relationship("User", back_populates="profile")
    experiences = relationship("Experience", back_populates="profile", cascade="all, delete-orphan")
    educations = relationship("Education", back_populates="profile", cascade="all, delete-orphan")
    skills = relationship("Skill", secondary=lambda: profile_skills, back_populates="profiles")
    locations = relationship("Location", back_populates="profile")

profile_skills = Table('profile_skills', Base.metadata,
    Column('profile_id', PG_UUID(as_uuid=True), ForeignKey('talent_profiles.id')),
    Column('skill_id', PG_UUID(as_uuid=True), ForeignKey('skills.id')))

class Experience(Base):
    __tablename__ = "experiences"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    profile_id = Column(PG_UUID(as_uuid=True), ForeignKey('talent_profiles.id'))
    company = Column(String(200), nullable=False)
    title = Column(String(200), nullable=False)
    start_date = Column(Date, nullable=False)
    end_date = Column(Date, nullable=True)
    is_current = Column(Boolean, default=False)
    description = Column(Text, nullable=True)
    achievements = Column(Text, default='[]')  # JSON for prototype
    created_at = Column(DateTime, default=datetime.utcnow)
    profile = relationship("TalentProfile", back_populates="experiences")

class Education(Base):
    __tablename__ = "education"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    profile_id = Column(PG_UUID(as_uuid=True), ForeignKey('talent_profiles.id'))
    institution = Column(String(200), nullable=False)
    degree = Column(String(200), nullable=False)
    field = Column(String(200), nullable=False)
    graduation_year = Column(Integer, nullable=False)
    created_at = Column(DateTime, default=datetime.utcnow)
    profile = relationship("TalentProfile", back_populates="educations")

class Skill(Base):
    __tablename__ = "skills"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    name = Column(String(100), unique=True, nullable=False)
    category = Column(String(100), nullable=True)
    verified = Column(Boolean, default=False)
    created_at = Column(DateTime, default=datetime.utcnow)
    profiles = relationship("TalentProfile", secondary=lambda: profile_skills, back_populates="skills")

class Location(Base):
    __tablename__ = "locations"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    profile_id = Column(PG_UUID(as_uuid=True), ForeignKey('talent_profiles.id'))
    city = Column(String(100), nullable=False)
    state = Column(String(100), nullable=True)
    country = Column(String(100), nullable=False)
    is_primary = Column(Boolean, default=True)
    profile = relationship("TalentProfile", back_populates="locations")

class AuditLog(Base):
    __tablename__ = "audit_logs"
    id = Column(PG_UUID(as_uuid=True), primary_key=True, default=uuid4)
    user_id = Column(PG_UUID(as_uuid=True), ForeignKey('users.id'), index=True)
    action = Column(String(100), nullable=False)
    resource_type = Column(String(50), nullable=False)
    resource_id = Column(PG_UUID(as_uuid=True), nullable=True)
    ip_address = Column(String(45), nullable=True)
    user_agent = Column(Text, nullable=True)
    details = Column(Text, nullable=True)
    timestamp = Column(DateTime, default=datetime.utcnow, index=True)
    user = relationship("User")

# Schemas
class UserBase(BaseModel):
    email: EmailStr
    role: UserRole

class UserCreate(UserBase):
    password: str
    organization_id: Optional[UUID] = None

class UserResponse(UserBase):
    id: UUID
    is_active: bool
    created_at: datetime
    last_login_at: Optional[datetime]
    class Config: from_attributes = True

class Token(BaseModel):
    access_token: str
    refresh_token: str
    token_type: str = "bearer"
    expires_in: int

class ExperienceCreate(BaseModel):
    company: str = Field(..., min_length=1, max_length=200)
    title: str = Field(..., min_length=1, max_length=200)
    start_date: date
    end_date: Optional[date] = None
    is_current: bool = False
    description: Optional[str] = None
    achievements: List[str] = []

    @validator('end_date')
    def end_after_start(cls, v, values):
        if 'start_date' in values and v and v < values['start_date']:
            raise ValueError('End date must be after start date')
        return v

class EducationCreate(BaseModel):
    institution: str = Field(..., min_length=1, max_length=200)
    degree: str = Field(..., min_length=1, max_length=200)
    field: str = Field(..., min_length=1, max_length=200)
    graduation_year: int = Field(..., ge=1900, le=datetime.now().year + 5)

class LocationCreate(BaseModel):
    city: str = Field(..., min_length=1, max_length=100)
    state: Optional[str] = Field(None, max_length=100)
    country: str = Field(..., min_length=1, max_length=100)
    is_primary: bool = True

class TalentProfileCreate(BaseModel):
    first_name: str = Field(..., min_length=1, max_length=100)
    last_name: str = Field(..., min_length=1, max_length=100)
    phone: Optional[str] = Field(None, regex=r'^\+?1?\d{9,15}$')
    summary: Optional[str] = Field(None, max_length=2000)
    resume_url: Optional[str] = None
    visibility: ProfileVisibility = ProfileVisibility.PUBLIC
    skills: List[str] = []
    experience: List[ExperienceCreate] = []
    education: List[EducationCreate] = []
    locations: List[LocationCreate] = []

    @validator('skills')
    def skills_non_empty(cls, v):
        if not v:
            raise ValueError('At least one skill is recommended')
        return [s.strip() for s in v if s.strip()]

class TalentProfileUpdate(BaseModel):
    first_name: Optional[str] = Field(None, min_length=1, max_length=100)
    last_name: Optional[str] = Field(None, min_length=1, max_length=100)
    phone: Optional[str] = Field(None, regex=r'^\+?1?\d{9,15}$')
    summary: Optional[str] = Field(None, max_length=2000)
    resume_url: Optional[str] = None
    visibility: Optional[ProfileVisibility] = None
    skills: Optional[List[str]] = None
    experience: Optional[List[ExperienceCreate]] = None
    education: Optional[List[EducationCreate]] = None
    locations: Optional[List[LocationCreate]] = None

class TalentProfileResponse(BaseModel):
    id: UUID
    user_id: UUID
    first_name: str
    last_name: str
    email: str
    phone: Optional[str]
    summary: Optional[str]
    resume_url: Optional[str]
    visibility: ProfileVisibility
    completeness_score: float
    created_at: datetime
    updated_at: datetime
    skills: List[str]
    experience_count: int
    education_count: int
    location_count: int
    class Config: from_attributes = True

class SearchQuery(BaseModel):
    keywords: Optional[List[str]] = None
    skills: Optional[List[str]] = None
    location: Optional[str] = None
    experience_range: Optional[Dict[str, int]] = None
    page: int = 1
    limit: int = 20
    sort_by: str = "relevance"

class SearchResult(BaseModel):
    total: int
    page: int
    limit: int
    results: List[Dict[str, Any]]

class Message(BaseModel):
    id: UUID
    conversation_id: UUID
    sender_id: UUID
    content: str
    message_type: str = "text"
    timestamp: datetime
    read: bool = False

class Conversation(BaseModel):
    id: UUID
    participants: List[UUID]
    created_at: datetime
    updated_at: datetime
    last_message: Optional[Dict] = None

# Services
class AuthService:
    def __init__(self):
        self.jwt_secret = settings.JWT_SECRET
        self.jwt_algorithm = "HS256"
        self.access_token_expire_minutes = 15
        self.refresh_token_expire_days = 7

    async def create_user(self, db: AsyncSession, user_data: UserCreate) -> User:
        existing = await self.get_user_by_email(db, user_data.email)
        if existing: raise ValueError("User exists")
        hashed = bcrypt.hashpw(user_data.password.encode(), bcrypt.gensalt()).decode()
        user = User(email=user_data.email, hashed_password=hashed, role=user_data.role, organization_id=user_data.organization_id)
        db.add(user)
        await db.commit()
        await db.refresh(user)
        return user

    async def authenticate_user(self, db: AsyncSession, email: str, password: str) -> Optional[User]:
        user = await self.get_user_by_email(db, email)
        if not user or not user.is_active or not bcrypt.checkpw(password.encode(), user.hashed_password.encode()):
            return None
        user.last_login_at = datetime.utcnow()
        await db.commit()
        return user

    def create_tokens(self, user: User) -> Token:
        access_expires = timedelta(minutes=self.access_token_expire_minutes)
        refresh_expires = timedelta(days=self.refresh_token_expire_days)
        access = jwt.encode({"sub": str(user.id), "role": user.role, "type": "access", "exp": datetime.utcnow() + access_expires}, self.jwt_secret, self.jwt_algorithm)
        refresh = jwt.encode({"sub": str(user.id), "type": "refresh", "exp": datetime.utcnow() + refresh_expires}, self.jwt_secret, self.jwt_algorithm)
        return Token(access_token=access, refresh_token=refresh, expires_in=int(access_expires.total_seconds()))

    def verify_token(self, token: str) -> Optional[Dict]:
        try:
            return jwt.decode(token, self.jwt_secret, algorithms=[self.jwt_algorithm])
        except jwt.JWTError:
            return None

    async def get_user_by_email(self, db: AsyncSession, email: str) -> Optional[User]:
        result = await db.execute(select(User).where(User.email == email))
        return result.scalar_one_or_none()

class TalentDataService:
    def __init__(self):
        self.s3_client = MockS3()
        self.search_service = None  # Link later

    async def create_profile(self, db: AsyncSession, user_id: UUID, profile_data: TalentProfileCreate) -> TalentProfile:
        profile = TalentProfile(user_id=user_id, **profile_data.dict(exclude={'skills', 'experience', 'education', 'locations'}))
        db.add(profile)
        await db.flush()
        # Stub adds for skills, exp, edu, loc
        if profile_data.skills: await self._add_skills_to_profile(db, profile, profile_data.skills)
        if profile_data.experience: await self._add_experience_to_profile(db, profile, profile_data.experience)
        # ... similar for education, locations
        await db.commit()
        await db.refresh(profile)
        await self._index_profile_in_search(profile)
        return profile

    async def update_profile(self, db: AsyncSession, user_id: UUID, updates: TalentProfileUpdate) -> TalentProfile:
        result = await db.execute(select(TalentProfile).where(TalentProfile.user_id == user_id))
        profile = result.scalar_one_or_none()
        if not profile: raise ValueError("Profile not found")
        for field, value in updates.dict(exclude_unset=True, exclude={'skills', 'experience', 'education', 'locations'}).items():
            setattr(profile, field, value)
        profile.updated_at = datetime.utcnow()
        # Stub updates
        await db.commit()
        await db.refresh(profile)
        await self._index_profile_in_search(profile)
        return profile

    async def upload_resume(self, user_id: UUID, file_content: bytes, filename: str) -> str:
        file_key = f"resumes/{user_id}/{uuid4()}-{filename}"
        self.s3_client.put_object(Bucket='mock', Key=file_key, Body=file_content)
        await self._trigger_cv_parsing(user_id, file_key)
        return f"https://mock.s3/{file_key}"

    async def _trigger_cv_parsing(self, user_id: UUID, file_key: str): pass
    async def _index_profile_in_search(self, profile: TalentProfile): pass
    async def _add_skills_to_profile(self, db: AsyncSession, profile: TalentProfile, skill_names: List[str]): pass
    async def _add_experience_to_profile(self, db: AsyncSession, profile: TalentProfile, experiences: List[Dict]): pass
    # ... similar stubs

class SearchService:
    def __init__(self):
        self.es_client = MockElasticsearch()

    async def search_talent(self, query: SearchQuery) -> SearchResult:
        search_body = self._build_search_query(query)
        result = await self.es_client.search(index="talent-profiles", body=search_body)
        return self._format_search_results(result, query)

    def _build_search_query(self, query: SearchQuery) -> Dict[str, Any]:
        # Stub basic query
        return {"query": {"match_all": {}}}

    def _get_sort_criteria(self, sort_by: str) -> List[Dict]: return [{"_score": {"order": "desc"}}]
    def _format_search_results(self, result: Dict, query: SearchQuery) -> SearchResult:
        return SearchResult(total=0, page=query.page, limit=query.limit, results=[])

    async def index_profile(self, profile_data: Dict): await self.es_client.index(index="talent-profiles", id=profile_data["id"], body=profile_data)
    async def remove_profile(self, profile_id: str): await self.es_client.delete(index="talent-profiles", id=profile_id)

class LLMService:
    def __init__(self):
        self.openai_client = MockOpenAI()
        self.pinecone = MockPinecone()

    async def generate_profile_summary(self, profile_data: Dict) -> str:
        # Stub
        return "Mock summary generated via GPT."

    async def match_candidates_to_job(self, job_description: str, candidate_profiles: List[Dict]) -> List[Dict]:
        # Stub matches
        return [{"profile": p, "score": 0.8, "reasoning": "Mock match"} for p in candidate_profiles[:5]]

    # ... other methods stubbed similarly

class CommunicationService:
    def __init__(self):
        self.redis_client = MockRedis()
        self.active_connections: Dict[str, WebSocket] = {}

    async def create_conversation(self, participant_ids: List[UUID]) -> Conversation:
        conv_id = uuid4()
        conv = Conversation(id=conv_id, participants=participant_ids, created_at=datetime.utcnow(), updated_at=datetime.utcnow())
        await self.redis_client.hset(f"conversation:{conv_id}", mapping={"id": str(conv_id), "participants": json.dumps([str(p) for p in participant_ids])})
        return conv

    async def send_message(self, conversation_id: UUID, sender_id: UUID, content: str, message_type: str = "text") -> Message:
        msg = Message(id=uuid4(), conversation_id=conversation_id, sender_id=sender_id, content=content, message_type=message_type, timestamp=datetime.utcnow())
        await self.redis_client.hset(f"message:{msg.id}", mapping={"content": content, "timestamp": msg.timestamp.isoformat()})
        await self._notify_participants(conversation_id, sender_id, msg)
        return msg

    # ... other methods stubbed

    async def _notify_participants(self, conv_id: UUID, sender_id: UUID, msg: Message): pass
    def add_websocket_connection(self, user_id: UUID, ws: WebSocket): self.active_connections[str(user_id)] = ws
    def remove_websocket_connection(self, user_id: UUID): self.active_connections.pop(str(user_id), None)

class ComplianceService:
    def __init__(self):
        self.cipher_suite = type('Fernet', (), {'encrypt': lambda s, d: d, 'decrypt': lambda s, d: d})()  # Mock

    def encrypt_pii(self, data: str) -> str: return data
    def decrypt_pii(self, data: str) -> str: return data
    def anonymize_profile(self, profile_data: Dict) -> Dict:
        anon = profile_data.copy()
        anon["first_name"] = "Candidate"
        return anon

    async def log_audit_event(self, event_data: Dict): logger.info(f"Audit: {event_data}")

    # ... other methods stubbed

# Middleware/Auth Deps
security = HTTPBearer()

async def get_current_user(credentials: HTTPAuthorizationCredentials = Depends(security), db: AsyncSession = Depends(get_db)) -> User:
    auth_service = AuthService()
    payload = auth_service.verify_token(credentials.credentials)
    if not payload or payload.get("type") != "access":
        raise HTTPException(status_code=401, detail="Invalid token")
    result = await db.execute(select(User).where(User.id == UUID(payload["sub"])))
    user = result.scalar_one_or_none()
    if not user or not user.is_active: raise HTTPException(status_code=401, detail="User inactive")
    return user

def require_role(allowed_roles: List[UserRole]):
    def role_checker(current_user: User = Depends(get_current_user)):
        if current_user.role not in allowed_roles:
            raise HTTPException(status_code=403, detail="Insufficient permissions")
        return current_user
    return role_checker

# Routers (inlined)
from fastapi import APIRouter
auth_router = APIRouter(prefix="/api/auth", tags=["Authentication"])
talent_router = APIRouter(prefix="/api/talent", tags=["Talent Management"])
search_router = APIRouter(prefix="/api/search", tags=["Search"])
communication_router = APIRouter(prefix="/api/communication", tags=["Communication"])
admin_router = APIRouter(prefix="/api/admin", tags=["Admin"])

@auth_router.post("/register", response_model=UserResponse)
async def register(user_data: UserCreate, db: AsyncSession = Depends(get_db)):
    auth_service = AuthService()
    try:
        user = await auth_service.create_user(db, user_data)
        return user
    except ValueError as e:
        raise HTTPException(status_code=400, detail=str(e))

@auth_router.post("/login", response_model=Token)
async def login(form_data: OAuth2PasswordRequestForm = Depends(), db: AsyncSession = Depends(get_db)):
    auth_service = AuthService()
    user = await auth_service.authenticate_user(db, form_data.username, form_data.password)
    if not user:
        raise HTTPException(status_code=401, detail="Invalid credentials")
    return auth_service.create_tokens(user)

@talent_router.post("/profiles", response_model=TalentProfileResponse)
async def create_profile(profile_data: TalentProfileCreate, current_user: User = Depends(require_role([UserRole.JOB_SEEKER])), db: AsyncSession = Depends(get_db)):
    service = TalentDataService()
    try:
        profile = await service.create_profile(db, current_user.id, profile_data)
        return profile
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@talent_router.get("/profiles/me", response_model=TalentProfileResponse)
async def get_my_profile(current_user: User = Depends(get_current_user), db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(TalentProfile).where(TalentProfile.user_id == current_user.id))
    profile = result.scalar_one_or_none()
    if not profile: raise HTTPException(status_code=404, detail="Not found")
    return profile

@talent_router.put("/profiles/me", response_model=TalentProfileResponse)
async def update_my_profile(updates: TalentProfileUpdate, current_user: User = Depends(require_role([UserRole.JOB_SEEKER])), db: AsyncSession = Depends(get_db)):
    service = TalentDataService()
    try:
        profile = await service.update_profile(db, current_user.id, updates)
        return profile
    except ValueError as e:
        raise HTTPException(status_code=404, detail=str(e))

@talent_router.post("/profiles/me/resume")
async def upload_resume(file: UploadFile = File(...), current_user: User = Depends(require_role([UserRole.JOB_SEEKER]))):
    if not file.content_type == 'application/pdf': raise HTTPException(status_code=400, detail="PDF only")
    service = TalentDataService()
    resume_url = await service.upload_resume(current_user.id, await file.read(), file.filename)
    return {"resume_url": resume_url}

@search_router.post("/talent/search", response_model=SearchResult)
async def search_talent(query: SearchQuery, current_user: User = Depends(require_role([UserRole.EMPLOYER, UserRole.RECRUITER]))):
    service = SearchService()
    try:
        results = await service.search_talent(query)
        return results
    except Exception as e:
        raise HTTPException(status_code=500, detail="Search failed")

@communication_router.post("/conversations", response_model=Conversation)
async def create_conversation(participants: List[UUID], current_user: User = Depends(get_current_user)):
    service = CommunicationService()
    conv = await service.create_conversation(participants)
    return conv

@communication_router.post("/messages", response_model=Message)
async def send_message(conversation_id: UUID, content: str, current_user: User = Depends(get_current_user)):
    service = CommunicationService()
    msg = await service.send_message(conversation_id, current_user.id, content)
    return msg

@admin_router.post("/compliance/retention")
async def enforce_retention(current_user: User = Depends(require_role([UserRole.ADMIN]))):
    service = ComplianceService()
    # Stub count
    return {"deleted": 0}

# App
@asynccontextmanager
async def lifespan(app: FastAPI):
    logger.info("Startup...")
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)
    yield
    logger.info("Shutdown...")

app = FastAPI(title="TalentHub", version="1.0.0", lifespan=lifespan)
app.add_middleware(TrustedHostMiddleware, allowed_hosts=settings.ALLOWED_HOSTS)
app.add_middleware(CORSMiddleware, allow_origins=settings.CORS_ORIGINS, allow_credentials=True, allow_methods=["*"], allow_headers=["*"])

@app.exception_handler(HTTPException)
async def http_exc_handler(req, exc):
    return JSONResponse(status_code=exc.status_code, content={"error": exc.detail})

@app.exception_handler(Exception)
async def gen_exc_handler(req, exc):
    logger.error(str(exc))
    return JSONResponse(status_code=500, content={"error": "Internal error"})

@app.get("/health")
async def health():
    return {"status": "healthy", "timestamp": datetime.utcnow().isoformat()}

app.include_router(auth_router)
app.include_router(talent_router)
app.include_router(search_router)
app.include_router(communication_router)
app.include_router(admin_router)

if __name__ == "__main__":
    uvicorn.run("talent_hub:app", host="0.0.0.0", port=8000, reload=True)