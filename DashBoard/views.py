from django.core.paginator import Paginator
from django.shortcuts import render, get_object_or_404
from .models import MemberInfo, MemberImageInfo

def home_view(request):
    return render(request, 'home.html')

def about_view(request):
    return render(request, 'about.html')

def rate_view(request):
    query = request.GET.get('q', '')
    if query:
        members = MemberInfo.objects.filter(
            elected_type='비례대표',
            name__icontains=query
        ).order_by('name')
    else:
        members = MemberInfo.objects.filter(
            elected_type='비례대표'
        ).order_by('name')
    paginator = Paginator(members, 15)
    page_number = request.GET.get('page')
    page_obj = paginator.get_page(page_number)
    return render(request, 'rate.html', {'page_obj': page_obj, 'query': query})

def local_view(request):
    query = request.GET.get('q', '')
    if query:
        members = MemberInfo.objects.filter(
            elected_type='지역구',
            name__icontains=query
        ).order_by('name')
    else:
        members = MemberInfo.objects.filter(
            elected_type='지역구'
        ).order_by('name')
    paginator = Paginator(members, 15)
    page_number = request.GET.get('page')
    page_obj = paginator.get_page(page_number)
    return render(request, 'local.html', {'page_obj': page_obj, 'query': query})

def map_view(request):
    return render(request, 'map.html')

def detail_view(request, member_code):
    member = get_object_or_404(MemberInfo, member_code=member_code)
    return render(request, 'detail.html', {'member': member,})

def read_view(request):
    return render(request, 'read.html')

def social_view(request):
    return render(request, 'social.html')

def social2_view(request):
    return render(request, 'social2.html')
