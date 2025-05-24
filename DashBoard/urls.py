from django.urls import path
from .views import home_view, about_view, rate_view, local_view, map_view, detail_view, read_view, social_view, social2_view, post_detail
from . import views

urlpatterns = [
    path('', home_view, name='home'),
    path('about/', about_view, name='about'),
    path('rate/', rate_view, name='rate'),
    path('local/', local_view, name='local'),
    path('map/', map_view, name='map'),
    path('member/<str:member_code>/', detail_view, name='member_detail'),    path('read/', read_view, name='read'),
    path('social2/', social2_view, name='social2'),
    path('social/', views.social, name='social'),
    path('social/<int:post_id>/', post_detail, name='post_detail'),

]